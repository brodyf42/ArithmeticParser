// AP_sample_usage.cpp
// Author: Brody Fischer
#include "ArithmeticParser.h"
#include <iostream>
#include <string>

using namespace std;

int main(){
	ap::Expression expression;
	string user_input;
	double value;

	cout << "Simple Arithmetic Parser" << endl;
	cout << "Please provide an arithmetic expression: ";
	getline(cin, user_input);

	expression.setExpression(user_input);

	if(expression.isValid()){
		cout << "The expression evaluated to: " << expression.getValue() << endl;
	}else{
		cout << "Unable to resolve given expression" << endl;
		cout << "Error Message: " << expression.getErrorMessage() << endl;
	}

	return 0;
}
