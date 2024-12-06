### THE PORT PROGRAMMING LANGUAGE

A very lightweight interpreted language designed to be run by my own runtime which has a less than 2KB of data. It is designed also to be efficient and to have the same level of control as languagse like C, C++ or Rust and yet be interpreted. I originally wanted to make this language as the language for a software that could dectect what your code is doing because inductive reasoning. It is supposed to extended thru extensions, libraries and dialects. I implemented it myself in C++ in less than a month working a lot on this language.

I still want to work on this feature where you could give it a .portc (PortCompiled) code which is executed by the Port Runtime and then it will give you source code back with things like comments and variable and function names. The way this feature is intended to work is thru a series of deductions which then produces something a description can be given to an AI large language model. For example, the AI is given the description F(n) = F(n-1) + F(n-2) and it outputs "Fibonacci Sequence".

But as I got more into coding I really got into creating my own custom programming language as another project on its own.

I may one day implement more advanced features for the compiler like Just-In-Time compilation and others.

This language is actually faster at cauculating the Fibonacci numbers than Python but Python takes a shorter amount of time to display them to the screen.

# Documentation
## Comments
If you want to comment the entire line use ```//COMMENT LINE``` and if not,  ```/*COMMENT*/```

## Operands
```
==
>
<
+
-
*
/
% (Modular arithmetic)
. (Acessing the nth element of an array)
& (equivalent to both bitwise & and boolean && in most programming languages)
| (equivalent to both bitwise | and boolean || in most programming languages)
```

## Variables
Variables are a-z and A-Z only no extra stuff atleast right now. They are either ```word``` or ```byte``` with the ```unsigned``` before the declaration keyword (```word``` or ```byte```) specifying them as unsigned (e.g: positive number or 0) you can also add ```array [CONSTANT]``` before the declaration keyword. After the declaration there can't be a ```= [VALUE]``` like in most other programmming languages.
```unsigned word CONSOLE```

## Expression
Atomic expressions are either just a variable, a number or the combination an operand like +, -, *, /, % and == plus any combination of the any of the two mentioned before but you can also nest expressions inside expressions with paranthesis like ```(3+2)+4```.


## Number
Constants by defeault are represented in decimal in the code but you prefix them with ```0x``` the interpreted will interpret it as hexadecimal and if you instead type in ```0b``` the interpreter will interpret what you type in as binary numbers are all eiher 16-bit (word) or 8-bit (byte)

## Statements
```
if (EXP){
  some code here
}
```
```
cascade (EXP1){ //if - Checks if EXP1 is equal to true, if true than execute the code below, if not go check the next condition
  some code here
}(EXP2){ //else if - Checks if EXP2 is equal to true, if true than execute the code below, if not go check the next condition
  some code here
}(1){ //else - Checks if true/1 is equal to true which it always is, so if neither EXP1 nor EXP2 are true, this is executed
  some code here
}
```
```
while (EXP1){
  some code here
}
```
```
fn FUNCTIONNAME{
  some code here
}
```

## Instructions
```
result [VARIABLE NAME]
```
```
[VARIABLE NAME] = [EXPRESSION]
```
```
[VARIABLE NAME].[EXPRESSION]) = [EXPRESSION]
```
```
error [CHARACTERS] //Throws a custom error
```
```
char [CHARACTERS] //Prints to the screen
```
```
break //Breaks out of a while loop or function
```
```
call [FUNCTION NAME]
```
```
use [STRING that is interpreted as name of .dll] [STRING that is the function you want to call] [VARIABLE-NAME]
```
```
push [ARRAY-NAME] [CONSTANT] //The constant specifies the number of 0s that is going to be pushed to the array
```
```
delete [ARRAY-NAME] [CONSTANT] //The constant specififes the index that is going to be deleted
```
