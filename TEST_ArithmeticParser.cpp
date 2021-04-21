// TEST_ArithmeticParser.cpp
// Author: Brody Fischer

// Using the Catch2 unit test framework. The preprocessor define creates main()
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "ArithmeticParser.h"

// NOTE: Approx class from test framework is used for floating point comparison

TEST_CASE( "Expression declaration with default constructor" ) {
	ap::Expression expression1;
	REQUIRE( expression1.getExpression() == "" );
	REQUIRE( expression1.isValid() == false );
	REQUIRE( expression1.getErrorMessage() == ap::MSG_EMPTY_EXPRESSION );
}

TEST_CASE( "Expression declaration with simple valid input string" ) {
	std::string s = "5";
	ap::Expression expression2(s);
	REQUIRE( expression2.getExpression() == s );
	REQUIRE( expression2.isValid() == true );
	REQUIRE( expression2.getErrorMessage() == ap::MSG_NO_ERROR );
	REQUIRE( expression2.getValue() == Approx(5) );
}

TEST_CASE( "Expression declaration with complex valid input string" ) {
	std::string s = "-(-2 + 4.1) * 6 + (2.0 / +(-3 - 2))";
	ap::Expression expression3(s);
	REQUIRE( expression3.getExpression() == s );
	REQUIRE( expression3.isValid() == true );
	REQUIRE( expression3.getErrorMessage() == ap::MSG_NO_ERROR );
	REQUIRE( expression3.getValue() == Approx(-13) );
}

TEST_CASE( "Expression updated with setExpression() and valid input string" ) {
	std::string s = "3 + 4";
	ap::Expression expression4(s);
	s = "2 * -3";
	expression4.setExpression(s);
	REQUIRE( expression4.getExpression() == s );
	REQUIRE( expression4.isValid() == true );
	REQUIRE( expression4.getErrorMessage() == ap::MSG_NO_ERROR );
	REQUIRE( expression4.getValue() == Approx(-6) );
}

TEST_CASE( "Check invalid input expression formats" ){
	ap::Expression expression5;

	SECTION( "operator at start of string" ){
		expression5.setExpression("+");
		REQUIRE( expression5.isValid() == false );
		REQUIRE( expression5.getErrorMessage() == ap::MSG_VALUE_NOT_FOUND );
	}

	SECTION( "close parentheses at start of string" ){
		expression5.setExpression(")");
		REQUIRE( expression5.isValid() == false );
		REQUIRE( expression5.getErrorMessage() == ap::MSG_VALUE_NOT_FOUND );
	}

	SECTION( "empty parentheses" ){
		expression5.setExpression("()");
		REQUIRE( expression5.isValid() == false );
		REQUIRE( expression5.getErrorMessage() == ap::MSG_VALUE_NOT_FOUND );
	}

	SECTION( "consecutive operators" ){
		expression5.setExpression("6 + * 5");
		REQUIRE( expression5.isValid() == false );
		REQUIRE( expression5.getErrorMessage() == ap::MSG_VALUE_NOT_FOUND );
	}

	SECTION( "consecutive values - with space" ){
		expression5.setExpression("7 + 8 9");
		REQUIRE( expression5.isValid() == false );
		REQUIRE( expression5.getErrorMessage() == ap::MSG_OPERATOR_NOT_FOUND );
	}

	SECTION( "consecutive values - invalid flaoting point number" ){
		expression5.setExpression("7.8.9");
		REQUIRE( expression5.isValid() == false );
		REQUIRE( expression5.getErrorMessage() == ap::MSG_OPERATOR_NOT_FOUND );
	}

	SECTION( "no operator between value and open parentheses" ){
		expression5.setExpression("4(3 + 2)");
		REQUIRE( expression5.isValid() == false );
		REQUIRE( expression5.getErrorMessage() == ap::MSG_OPERATOR_NOT_FOUND );
	}

	SECTION( "no value between operator and close parentheses" ){
		expression5.setExpression("(3 + )");
		REQUIRE( expression5.isValid() == false );
		REQUIRE( expression5.getErrorMessage() == ap::MSG_VALUE_NOT_FOUND );
	}

	SECTION( "unbalanced parentheses in otherwise valid expression" ){
		SECTION( "doubled open parentheses" ){
			expression5.setExpression("((1 + 1)");
			REQUIRE( expression5.isValid() == false );
			REQUIRE( expression5.getErrorMessage() == ap::MSG_UNBALANCED_PARENS );
		}

		SECTION( "doubled close parentheses" ){
			expression5.setExpression("(1 + 1))");
			REQUIRE( expression5.isValid() == false );
			REQUIRE( expression5.getErrorMessage() == ap::MSG_UNBALANCED_PARENS );
		}

		SECTION( "only open parentheses" ){
			expression5.setExpression("(1 + 1");
			REQUIRE( expression5.isValid() == false );
			REQUIRE( expression5.getErrorMessage() == ap::MSG_UNBALANCED_PARENS );
		}

		SECTION( "only close parentheses" ){
			expression5.setExpression("1 + 1)");
			REQUIRE( expression5.isValid() == false );
			REQUIRE( expression5.getErrorMessage() == ap::MSG_UNBALANCED_PARENS );
		}
	}
}

TEST_CASE( "check infinity values" ){
	ap::Expression expression6;

	SECTION ( "number larger than max double value" ){
		expression6.setExpression("100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 * 2");
		CHECK( expression6.isValid() == false );
		CHECK( expression6.getErrorMessage() == ap::MSG_INFINFITE_RESULT );
	}

	SECTION ( "division by zero" ){
		expression6.setExpression("6 / 0");
		REQUIRE( expression6.isValid() == false );
		REQUIRE( expression6.getErrorMessage() == ap::MSG_INFINFITE_RESULT );
	}
}
