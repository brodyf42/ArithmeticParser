# Arithemtic Parser

This is a tool for parsing simple arithmetic expressions implemented with C++.  The *Expression* class is the focal entity and is defined by an input string representing an arithmetic expression that can be resolved to a single value. For example, the input string `(5 - 3) * 6` would be resolved to a value of `12`.

## Parsing Input

Once an expression string is provided, it is automatically parsed into a list of indivisible components, including opening parentheses, closing parentheses, values, and operators. These are represented by derivatives of the *Token* struct.

Values can be represented with or without sign characters and as simple decimals. Decimal values must have a digit before the decimal point (e.g. 0.5).  Exponential formats like `-2.3e+5` are not supported.  All values are stored as double floating-point types for simplicity.

The only operators currently supported are addition(`+`), subtraction(`-`), multiplication(`*`), and division(`/`).  Additionally, only expressions formatted using infix notation are considerd valid.

Generally speaking, whitespace in the input string is ignored. This means that from the perspective of the parsing algorithm, most given strings are equivalent to the same string with all whitespace removed.

The only exceptions to this rule are explicit unary sign characters `+` and `-` that precede opening parentheses or values. If whitespace existed between these characters and their intended operand, then the parse operation would fail.

examples of *valid* input:

`(1 + 2) * (3 + -4.0)`

`(1+2)*(3+-4.0)`

`-(5 + 6)`

examples of *invalid* input:

`(1 + 2) * (3 + - 4.0)`

`- (5 + 6)`

The parsing algorithm implements a simple state machine to validate the input. If an invalid format is encountered during parsing, an error flag and message are set to appropriate values that can be checked via public member functions.

## Resolving a Value

The *Expression* class maintains a private vector that holds a list of pointers to the abstract *Token* struct type. Once this vector is populated by the string parser, then arithmetic evaluation is done using this list of tokens.

The list of tokens remains unaltered throughout value resolution. The functions responsible for evaluating the tokens pass position indices that reference some portion of the token list.

This is accomplished with 2 mutually recursive functions, one that resolves an actual value, and one that manages the arithmetic operations. Both functions make use of a third function in order to handle parenthetical sub-expressions.

Since the string parsing algorithm should flag any formatting errors, this token resolution function and the supporting functions will throw exceptions if unexpected token types are encountered during execution.

## Using the Class

See the ***AP_sample_usage.cpp*** file for example usage of the *Expression* class that includes simple instantiation, expression setting, and error checking.

## Included Files

***ArithmeticParser.h*** defines the "ap" namespace containing the following:
- The *Expression* class definition
- The *Token* abstract struct and derived struct definitions
- A variety of constant values and enumerations used by *Expression* and *Token*

***ArithmeticParser.cpp*** contains the implementation of the *Expression* class that was defined in ArithmeticParser.h.

***AP_sample_usage.cpp*** contains a simple main() function that utilizes the *Expression* class defined in the previous files to provide an example use case.

***TEST_ArithmeticParser.cpp*** utilizes the Catch2 library to perform unit testing on the Expression class.

## Third Party Libraries

[Catch2](https://github.com/catchorg/Catch2) is used for unit testing.

***catch.hpp*** and ***LICENSE_1_0.txt*** are included for this purpose.
