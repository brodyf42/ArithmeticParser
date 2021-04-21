// ArithmeticParser.cpp
// Author: Brody Fischer
#include "ArithmeticParser.h"
using namespace ap;

/*** CONSTRUCTORS AND DESTRUCTOR ***/

// An Expression created with the default constructor
// uses the empty string and is not considered valid.
Expression::Expression() : Expression("") {}

Expression::Expression(const std::string & exp) : _exp(exp){
	evaluateExpression();
}

Expression::~Expression(){
	clearTokenList();
}

/*** PUBLIC FUNCTIONS ***/

void Expression::setExpression(const std::string & exp) {
	 _exp = exp;
	 evaluateExpression();
 }

// Accessors

std::string Expression::getExpression() const { return _exp; }

bool Expression::isValid() const { return _is_valid; }

std::string Expression::getErrorMessage() const { return _error_message; }

// value is meaningless if _is_valid is false
double Expression::getValue() const { return _value; };

/*** PRIVATE FUNCTIONS ***/

// This top-level function is the entry point to the parsing algorithms
// when an Expression is instantiated or a new expression string is set
// NOTE: std::logic_error is thrown by the called functions in invalid states
void Expression::evaluateExpression(){
	// flag error if expression is empty, otherwise create the token list
	if(_exp.empty()){
		_is_valid = false;
		_error_message = MSG_EMPTY_EXPRESSION;
	}else{
		try{
			createTokenList();
		}catch(const std::exception & e){
			clearTokenList();
			_is_valid = false;
			_error_message = e.what();
			throw;
		}
	}

	// if token list creation was successful, resolve it to a value
	if(_is_valid){
		try{
			_value = resolveTokenList(0, _token_list.size()-1);
		}catch(const std::exception & e){
			clearTokenList();
			_is_valid = false;
			_error_message = e.what();
			throw;
		}
	}

	// flag error if result is 'infinity'
	// this indicates overflow or division by zero
	if(std::isinf(_value)){
		_is_valid = false;
		_error_message = MSG_INFINFITE_RESULT;
	}
}

// This function implements the state machine that either parses the expression
// string into a valid list of tokens to be evaluated arithmetically or flags an
// error and sets an appropriate error message.
void Expression::createTokenList(){
	clearTokenList();
	int paren_balance = 0;
	std::string exp = _exp;
    std::smatch match;

	// This ensures that only the beginning of the expression string is
	// checked for a match when it is included in a regex method call.
	// As matches are found, they are removed from the expression string.
	std::regex_constants::match_flag_type continuous = std::regex_constants::match_continuous;

	// the entry point into the state machine is the FIND_OPEN_PAREN state
	parse_state state = FIND_OPEN_PAREN;

	while(state != FINISHED){
		switch(state){
		case FIND_OPEN_PAREN:
			// If an open parentheses is found, queue appropriate tokens and look
			// for another open parentheses. Otherwise, look for a value.
			if( std::regex_search(exp, match, patterns.at("open_paren"), continuous) ){
				// if a minus sign is present in the match, the whole
				// parenthetical expression is multiplied by -1. We can just queue
				// -1 and * tokens since multiplication is in the highest precedence
				if( std::regex_match(match.str(0), patterns.at("minus")) ){
					_token_list.push_back( new ValueToken(-1) );
					_token_list.push_back( new OperatorToken(MUTILPICATION) );
				}
				_token_list.push_back( new OpenParenToken );
				++paren_balance;
				exp = match.suffix();
			}else{
				state = FIND_VALUE;
			}
			break;

		case FIND_VALUE:
			// If a value is found, convert the string to a double and add a
			// token to the list. Otherwise, the input expression is invalid.
			if( std::regex_search(exp, match, patterns.at("value"), continuous) ){
				_token_list.push_back( new ValueToken(std::stod(match.str(0))) );
				exp = match.suffix();
				state = FIND_CLOSE_PAREN;
			}else{
				_is_valid = false;
				_error_message = MSG_VALUE_NOT_FOUND;
				state = FINISHED;
			}
			break;

		case FIND_CLOSE_PAREN:
			// If a close parentheses is found, queue appropriate token and look
			// for another close parentheses. Otherwise, look for an operator.
			if( std::regex_search(exp, match, patterns.at("close_paren"), continuous) ){
				_token_list.push_back( new CloseParenToken );
				--paren_balance;
				exp = match.suffix();
			}else{
				state = FIND_OPERATOR;
			}
			break;

		case FIND_OPERATOR:
			// If an operator is found, queue appropriate token based on string
			// and look for an open parentheses. Otherwise, exit main state loop.
			if( std::regex_search(exp, match, patterns.at("operator"), continuous) ){
				_token_list.push_back( getOpTokenFromString(match.str(0)) );
				exp = match.suffix();
				state = FIND_OPEN_PAREN;
			}else{
				state = OPERATOR_NOT_FOUND;
			}
			break;

		case OPERATOR_NOT_FOUND:
			// If the end of the expression string has been reached, or only
			// whitespace remains, that indicates succesfull completion.
			// Otherwise, the input expression is invalid.
			if( exp.empty() || std::regex_match(exp, patterns.at("whitespace")) ){
				// Even though the expression was successfully parsed,
				// there may still be invalid parentheses usage
				if(paren_balance != 0){
					_is_valid = false;
					_error_message = MSG_UNBALANCED_PARENS;
				}else{
					_is_valid = true;
					_error_message = MSG_NO_ERROR;
				}
			}else{
				_is_valid = false;
				_error_message = MSG_OPERATOR_NOT_FOUND;
			}

			state = FINISHED;
			break;
		}
	}
}

// This function is mutually recursive with resolveValueAtPos
// This function manages the arithmetic operations while resolveValueAtPos
// actually provides the termination point for the recursive calls
double Expression::resolveTokenList(const int & start_pos, const int & end_pos){
	int completed_pos = end_pos + 1;
	int current_pos = start_pos;
	int next_pos;
	double result;
	token_type cur_token_type = _token_list[current_pos]->getType();

	// Handle initial value or sub expression before looping through the list.
	// Any other token type would be invalid and should not be present.
	// The resolveValueAtPos function handles that case.
	result = resolveValueAtPos(current_pos);
	current_pos = getSubExpressionEndPos(current_pos) + 1;

	// This loop should always be looking at operators since values and
	// sub expressions are handled by resolveValueAtPos.
	while(current_pos < completed_pos){
		cur_token_type = _token_list[current_pos]->getType();

		if(cur_token_type == OPERATOR){
			// checking the token at current_pos+1 should be safe since an
			// operator cannot be the last token in a valid expression
			next_pos = current_pos + 1;

			switch ( ((OperatorToken*)_token_list[current_pos])->function ){

			// for addition and subtraction, we can evaluate the rest
			// of the list first due to the associative property and
			// becasue these operations have the lowest precedence
			case ADDITION:
				result += resolveTokenList(next_pos, end_pos);
				current_pos = completed_pos;
				break;
			case SUBTRACTION:
				result -= resolveTokenList(next_pos, end_pos);
				current_pos = completed_pos;
				break;

			// for multiplication and division, we can perform the operation with
			// the next value or sub expression then move on to the next operator
			// since these operations have the highest precedence
			case MUTILPICATION:
				result *= resolveValueAtPos(next_pos);
				current_pos = getSubExpressionEndPos(next_pos) + 1;
				break;
			case DIVISION:
				result /= resolveValueAtPos(next_pos);
				current_pos = getSubExpressionEndPos(next_pos) + 1;
				break;
			}
		}else{
			throw std::logic_error(EXMSG_UNEXPECTED_TOKEN);
		}
	}

	return result;
}

// This function is mutually recursive with resolveTokenList
// If a value token is at the given position, then the value is returned.
// If it's an open paren token, the sub expression is evalueated and returned.
// If any other token type is at position, then an error has occurred.
double Expression::resolveValueAtPos(const int & position){
	double result;
	token_type type = _token_list[position]->getType();

	if(type == VALUE){
		result = ((ValueToken*)_token_list[position])->value;
	}else if(type == OPEN_PAREN){
		// evaluate only the tokens inside the parentheses
		result = resolveTokenList(position + 1, getSubExpressionEndPos(position) - 1);
	}else{
		throw std::logic_error(EXMSG_UNEXPECTED_TOKEN);
	}

	return result;
}

// Given the position of an opening parentheses token in the token list,
// this returns the position of the matching closing parentheses token.
// Given the position of any other token, this returns the input position.
int Expression::getSubExpressionEndPos(const int & start_pos){
	if(_token_list[start_pos]->getType() != OPEN_PAREN){
		return start_pos;
	}

	// start with positive balance since we know the first token is an open paren
	int paren_balance = 1;
	int current_pos = start_pos + 1;

	while(paren_balance != 0){
		switch(_token_list[current_pos]->getType()){
		case OPEN_PAREN:
			++paren_balance;
			break;
		case CLOSE_PAREN:
			--paren_balance;
			break;
		}
		++current_pos;
	}
	// return current_pos-1 since the value was incremented before ending the loop
	return current_pos - 1;
}

OperatorToken* Expression::getOpTokenFromString(const std::string & op_string){
	if( std::regex_match(op_string, patterns.at("minus")) )
		return new OperatorToken(SUBTRACTION);
	else if( std::regex_match(op_string, patterns.at("plus")) )
		return new OperatorToken(ADDITION);
	else if( std::regex_match(op_string, patterns.at("times")) )
		return new OperatorToken(MUTILPICATION);
	else if( std::regex_match(op_string, patterns.at("divide")) )
		return new OperatorToken(DIVISION);
	else
		throw std::logic_error(EXMSG_INVALID_OPERATOR);
}

// release memory for dynamically allocated tokens and empty the token list
void Expression::clearTokenList(){
	for(auto token : _token_list){
		delete token;
	}
	_token_list.clear();
}
