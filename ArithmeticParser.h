// ArithemticParser.h
// Author: Brody Fischer
#ifndef _ARITHMETIC_PARSER_H
#define _ARITHMETIC_PARSER_H

#include <string>
#include <regex>
#include <map>
#include <vector>
#include <cmath>
#include <stdexcept>

namespace ap{
	// regular expressions used for parsing input string
	static const std::map<std::string,std::regex> patterns = {
		{"value",		std::regex("\\s*[-+]?\\d+(\\.\\d+)?")},
		{"open_paren",	std::regex("\\s*[-+]?\\(")},
		{"close_paren",	std::regex("\\s*\\)")},
		{"operator",	std::regex("\\s*[-+*/]")},
		{"whitespace",	std::regex("\\s*")},
		{"minus",		std::regex(".*-.*")},
		{"plus",		std::regex(".*\\+.*")},
		{"divide",		std::regex(".*\\/.*")},
		{"times",		std::regex(".*\\*.*")}
	};

	// error messages generated if input is invalid or arithmetic overflows
	static const std::string MSG_EMPTY_EXPRESSION = "no expression was provided";
	static const std::string MSG_OPERATOR_NOT_FOUND = "expected arithmetic operator not found";
	static const std::string MSG_VALUE_NOT_FOUND = "expected value or open parentheses not found";
	static const std::string MSG_UNBALANCED_PARENS = "unmatched parentheses in expression";
	static const std::string MSG_INFINFITE_RESULT = "infinite result encountered: possible division by zero";
	static const std::string MSG_NO_ERROR = "";

	// messages reported in thrown exceptions if invalid logic branch is reached
	static const std::string EXMSG_INVALID_OPERATOR = "execution error: invalid operator encountered while parsing expression";
	static const std::string EXMSG_UNEXPECTED_TOKEN = "execution error: unexpected token type encountered during evaluation";

	// state machine state names for parsing input string
	enum parse_state { FIND_OPEN_PAREN, FIND_VALUE, FIND_CLOSE_PAREN, FIND_OPERATOR, OPERATOR_NOT_FOUND, FINISHED };

	// enumerated values for Token and derived structs
	enum token_type { OPEN_PAREN, CLOSE_PAREN, VALUE, OPERATOR };
	enum operator_function { ADDITION, SUBTRACTION, MUTILPICATION, DIVISION };

	// Token derivatives represent indivisible components of the expression
	// This includes opening and closing parentheses, values, and operators
	struct Token{ virtual token_type getType() = 0; };
	struct OpenParenToken : Token { virtual token_type getType() {return OPEN_PAREN;} };
	struct CloseParenToken : Token { virtual token_type getType() {return CLOSE_PAREN;} };

	struct ValueToken : Token {
		const double value;
		ValueToken(const double & val) : value(val) {}
		virtual token_type getType() { return VALUE; }
	};

	struct OperatorToken : Token {
		const operator_function function;
		OperatorToken(const operator_function & func) : function(func) {}
		virtual token_type getType() { return OPERATOR; }
	};

	class Expression {
		double _value;
        std::string _exp;
		std::vector<Token*> _token_list;
		bool _is_valid;
		std::string _error_message;
		void createTokenList();
		void clearTokenList();
		double resolveTokenList(const int &, const int &);
		double resolveValueAtPos(const int &);
		int getSubExpressionEndPos(const int &);
		OperatorToken* getOpTokenFromString(const std::string &);
		void evaluateExpression();
    public:
        Expression();
        Expression(const std::string &);
        ~Expression();
        void setExpression(const std::string &);
        std::string getExpression() const;
		bool isValid() const;
		std::string getErrorMessage() const;
		double getValue() const;
    };
}

#endif //end _ARITHMETIC_PARSER_H definition
