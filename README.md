# Mini C Interpreter (Recursive Descent Parser)

This project implements a simple C-like language interpreter using a recursive descent parser in C.

## Features

- Variable declaration
- Arithmetic expressions with operator precedence
- Print statements
- Syntax error detection
- Semantic error detection (undeclared variables, division by zero)

## Example Input
int y = 5;
int x = 20;
int z = x + y;
print(z);

## Output
25

## Grammar (CFG)

<program> → <statement_list>

<statement_list> → <statement> <statement_list> | ε

<statement> → <declaration> | <print_stmt>

<declaration> → int <identifier> = <expression> ;

<print_stmt> → print ( <identifier> ) ;

<expression> → <term> <expression'>
<expression'> → + <term> <expression'> | - <term> <expression'> | ε

<term> → <factor> <term'>
<term'> → * <factor> <term'> | / <factor> <term'> | ε

<factor> → <identifier> | <number>

## Compilation
gcc parser.c -o parser

## Execution
./parser input.txt


## Author
Jananganan Rajendran



