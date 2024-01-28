#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stack>
using namespace std;
unsigned int i = 0;
unsigned int j = 0;
unsigned int k = 0;
unsigned int length = 0;
unsigned int lineNumber = 0;
unsigned char compInt = 0;
unsigned int item = 0;
unsigned int compVar = 0;
unsigned char state = 0;
unsigned char nested = 0;
bool myBool = 0;
bool same = 1;
unsigned char chr = 0;
string code = "";
const string items = "if.ifelse.cascade.call.fn.while.return.unsigned.byte.word.const.array.result.char.push.delete.break.import.use.error.";
unsigned char opcodes[] = {0b11000010,0,0b11010000,0b11000000,0b11000100,0b11000011,0b11001100,0,0,0,0,0,0b11000111,0b11001101,0b11001001,0b11001000,0b11000001,0,0b11001011,0b11001111};
vector<char> result;
vector<char> functions;
vector<char> variables;
enum threePossibilities{
    IF,
    CASCADE,
    WHILE
};
stack<threePossibilities> boolStack;
//vector<char> expression;
void exitWithError(string error){
	cout << "\033[1;31m(X) Compiler ERROR Line " << lineNumber << ": " << error << "\033[0m";
	exit(EXIT_FAILURE);
}
int numberParse(){
	compInt = 0;
	if(code[i] == '-'){
		compInt = 0-compInt;
		i++;
	}
	if((code[i]=='0')){
		i++;
		i++;
		if(code[i-1]=='x'){
			while(true){
				if((code[i]>47)&&(code[i]<58)){
					compInt = compInt * 16;
					compInt = compInt + code[i]-48; // 0 1 2 3 4 5 6 7 8 9 (ASCII for 0)
				}else if((code[i]>64)&&(code[i]<71)){
					compInt = compInt * 16;
					compInt = compInt + code[i]-55; // A B C D E F -55 (ASCII for 0)
				}else{
					break;
				}
				i++;
			}
		}else if(code[i-1]=='b'){
			while(true){
				if(code[i]=='1'){
					compInt++;
					compInt = compInt * 2;
				}else if(code[i]=='0'){
					compInt = compInt * 2;
				}else{
					break;
				}
				i++;
			}
		}else{
			compInt = 0;
			i--;
		}
	}else{
		while(true){
			if((code[i]>47)&&(code[i]<58)){
				compInt = compInt * 10;
				compInt = compInt + code[i]-48; // 0 1 2 3 4 5 6 7 8 9
			}else{
				break;
			}
			i++;
		}
	}
}
int evalVarName(){
	j=0;
	k=0;
	compVar = 0;
	if((variables.size())==0){
		exitWithError("No variables defined");
	}
	same = 1;
	while(true){
		if(variables[k]==' '){
			if(same&&(code[i+j]<65)){
				break;
			}
			if(k==(variables.size()-1)){
				exitWithError("Undefined variable");
			}
			j=0;
			same = 1;
			k++;
			compVar++;
		}
		if(code[i+j]!=variables[k]){
			same = 0;
		}
		//cout << code[i+j];
		//cout << code[i+j] << '-' << variables[k] << '-' << '\n';
		k++;
		j++;
	}
	i=i+j;
}
void compileExp(){
	/*if(rootExpression){
		expression.clear();
	}*/
	bool secondOperand = 0;
	bool varOrNum = 0;
	bool nestedExp = 0;
	while((code[i]!=')')&&(code[i]!='\n')){
		//cout << code[i];
		if((code[i]>47)&&(code[i]<58)){
			numberParse();
			if(secondOperand == 1){
				result.push_back(0b11110111);
				result.push_back(compInt);
			}
		}else if((code[i]>64)&&(code[i]<123)){
			evalVarName();
			if(secondOperand == 1){
				if(nestedExp){
					result.push_back(compVar);
				}else{
					result.push_back(0b11111111);
					result.push_back(compVar);
				}
			}else{
				varOrNum = 1;
			}
		}else if(code[i]=='('){
			if(secondOperand == 1){
				i++;
				compileExp();
				i++;
			}else{
				nestedExp = 1;
				i++;
				compInt = i;
				nested = 0;
				while(true){
					if(code[i]==')'){
						if(nested==0){
							break;
						}
						nested--;
					}else if(code[i]=='('){
						nested++;
					}
					i++;
				}
				i++;
			}
		}else if(code[i]==' '){
			i++;
		}else{
			//cout << code[i] << '\n';
			switch(code[i]){
				case '=':
					if(code[i+1]=='='){
						result.push_back(0b10100000);
						i++;
					}
					break;
				case '>':
					result.push_back(0b10100001);
					break;
				case '<':
					result.push_back(0b10100010);
					break;
				case '+':
					result.push_back(0b10100011);
					break;
				case '-':
					result.push_back(0b10100100);
					break;
				case '.':
					result.push_back(0b10100101);
					break;
				case '*':
					if(code[i+1]=='*'){
						result.push_back(0b10101011);
						i++;
					}else{
						result.push_back(0b10100110);
					}
					break;
				case '/':
					result.push_back(0b10100111);
					break;
				case '%':
					result.push_back(0b10101000);
					break;
				case '&':
					result.push_back(0b10101001);
					break;
				case '|':
					result.push_back(0b10101010);
					break;
				default:
					exitWithError("Undefined operation");
					break;
			}
			if(nestedExp){
				i = compInt;
				compileExp();
				i++;
				if(code[i]=='='){
					i++;
				}
				nestedExp = 0;
			}else{
				nestedExp = 0;
				if(code[i]=='.'){
					result.push_back(compVar);
				}else if((code[i-1]=='*')&&(code[i]=='*')){
					nestedExp = 1; //We are using nestedExp to store if the operator is **
				}else{
					if(varOrNum){
						result.push_back(0b11111111);
						result.push_back(compVar);
					}else{
						result.push_back(0b11110111);
						result.push_back(compInt);
					}
				}
			}
			secondOperand = 1;
			i++;
		}
	}
	if(secondOperand == 0){
		if(varOrNum){
			result.push_back(0b11111111);
			result.push_back(compVar);
		}else{
			result.push_back(0b11110111);
			result.push_back(compInt);
		}
	}
}
int variable(){
	i++;
	result.push_back(chr);
	if(chr&2){
		if(chr&8){
			result.push_back(compInt);
			result.push_back(compInt/256);
		}else{
			result.push_back(compInt);
		}
	}
	//cout << "Created new variable ";
	while(code[i]!='\n'){
		if(code[i]<65){
			exitWithError("Character not allowed in variable name: " + code[i]);
		}
		variables.push_back(code[i]);
		// cout << code[i];
		i++;
	}
	variables.push_back(' ');
	chr = 0;
}
/*void clear(){
	j = 0;
	while((i<8)&&(char[i]!=0)){
		j++;
	}
}*/
bool compare(string any_str){
	j=0;
	while(any_str[j]==code[i+j]){
		j++;
		if(j==(any_str.length())){
			i=i+j;
			return true;
		}
	}
	return false;
}
int main(int argc, char* argv[]){
	ifstream file;
	file.open(argv[1]);
	if(file.fail()){
		cout << "ERROR: Cannot open file, please see if there's a typo somewhere";
		return 1;
	}
	stringstream buffer;
	buffer << file.rdbuf();
	code = buffer.str();
	code.push_back('\n');
	code.push_back('\0');
	lineNumber++;
	while(code[i]!='\0'){
		while(code[i]=='\t'){
			i++;
		}
		if(code[i]=='*'){
			result.push_back(0b11001010);
			i++;
		}
		cout << code[i] << '=';
		if((code[i]=='/')&&(code[i+1]=='/')){
			i=i+2;
			while(code[i]!='\n'){
				i++;
			}
		}else if((code[i]=='/')&&(code[i+1]=='*')){
			i=i+2;
			while((code[i]!='*')||(code[i+1]!='/')){
				i++;
			}
			i=i+2;
		}else if(code[i]=='}'){
			if(boolStack.empty()){
				exitWithError("} is terminating what?");
			}else{
				if(code[i+1]=='('){ //cascades
					result.push_back(0b11010011);
					boolStack.push(CASCADE);
					i=i+2;
					compileExp();
					i=i+2;
				}else if(boolStack.top() == IF){ //if and elses
					result.push_back(0b11001100);
				}else if(boolStack.top() == CASCADE){ //cascades2
					result.push_back(0b11010010);
				}else{ //functions and whiles
					result.push_back(0b11100000);
				}
			}
			boolStack.pop();
			i++;
		}else{
			j=i;
			k=0;
			length=0;
			item=0;
			myBool = 1;
			while(items[k]!='\0'){
				if(items[k]=='.'){
					//cout << myBool;
					if(myBool&&(code[j]<65)){
						i = j;
						same = 1;
						break;
					}
					j=i;
					k++;
					myBool = 1;
					item++;
				}
				//cout << items[k] << ' ' << code[j] << '\n';
				if(items[k]!=code[j]){
					myBool = 0;
				}
				j++;
				k++;
				length++;
			}
			//cout << item << '\n';
			if(item==20){
				result.push_back(0b11000101);
				//cout << code[i] << '_';
				evalVarName();
				result.push_back(compVar);
				if(code[i]=='.'){
					result[result.size()-2] = 0b11000110;
					i++;
					compileExp();
					i++;
				}
				i=i+3;
				compileExp();
			}else{
				if(opcodes[item] != 0){
					result.push_back(opcodes[item]);
				}
				switch(item){
					case 0: //if 
						//cout << '-' << code[i];
						i++;
						boolStack.push(IF);
						compileExp();
						i=i+2;
						break;
					case 1: //else
						break;
					case 2: //cascade
						i++;
						compileExp();
						i=i+2;
						boolStack.push(CASCADE);
						break;
					case 3:	//call
						i++;
						j=0;
						k=0;
						compVar = 0;
						same = 1;
						while(true){
							if((functions[k]==' ')&&(code[i+j]<65)){
								if(same){
									i=i+j;
									result.push_back(compVar);
									break;
								}
								if(k==(functions.size()-1)){
									exitWithError("Undefined function");
								}
								j=0;
								same = 1;
								k++;
								compVar++;
							}
							if(code[i+j]!=functions[k]){
								same = 0;
							}		
							//cout << code[i+j] << '-' << functions[k] << '-' << '\n';
							k++;
							j++;
						}
						break;
					case 4: //fn
						i++;
						boolStack.push(WHILE);
						while(code[i]!='{'){
							functions.push_back(code[i]);
							i++;
						}
						functions.push_back(' ');
						i++;
						//result.push_back(0b11000100);
						break;
					case 5: //while
						i++;
						boolStack.push(WHILE);
						compileExp();
						i=i+2;
						break;
					case 6: //return
						break;
					case 7: //unsigned
						chr = chr + 1;
						break;
					case 8: //byte
						variable();
						break;
					case 9: //word
						chr = chr + 0x10;
						variable();
						break;
					case 10: //const
						chr = chr + 4;
						variable();			
						break;
					case 11: //array
						chr = chr + 2;
						i++;
						numberParse();
						if(compInt>0xFF){
							chr = chr + 8;
						}
						break;
					case 12: //result
						i++;
						evalVarName();
						result.push_back(compVar);
						break;
					case 13: //char
						i++;
						while(code[i]!='\n'){
							//cout << code[i];
							if((code[i]=='\\')&&(code[i+1]=='n')){
								i++;
								result.push_back('\n');
							}else if((code[i]=='\\')&&(code[i+1]=='\\')){
								i++;
								result.push_back('\\');
							}else{
								result.push_back(code[i]);
							}
							i++;
						}
						result.push_back('\0');
						break;
					case 14: //push
						i++;
						//cout << code[i];
						evalVarName();
						result.push_back(compVar);
						i++;
						numberParse();
						result.push_back(compInt);
						break;
					case 15: //delete
						i++;
						evalVarName();
						result.push_back(compVar);
						i++;
						numberParse();
						result.push_back(compInt);
						break;
					case 16: //break
						break;
					case 17: //import
						break;
					case 18: //use
						i++;
						chr = ' ';
						if(code[i]=='"'){
							chr = '"';
							i++;
						}
						while(code[i]!=chr){
							//cout << code[i];
							result.push_back(code[i]);
							i++;
						}
						if(code[i]=='"'){
							i++;
						}
						result.push_back(1);
						i++;
						//cout << '_';
						while(code[i]!=' '){
							//cout << code[i];
							result.push_back(code[i]);
							i++;
						}
						i++;
						result.push_back('\0');
						evalVarName();
						result.push_back(compVar);
						break;
					case 19: //error
						i++;
						same = 0;
						if(code[i]=='"'){
							same = 1;
							i++;
						}
						if(same){
							while(code[i]!='"'){
								result.push_back(code[i]);
								i++;
							}
							i++;
						}else{
							while(code[i]!=' '){
								result.push_back(code[i]);
								i++;
							}
						}
						result.push_back('\0');
						break;
				}
			}
		}
		if(code[i]=='\n'){
			lineNumber++;
		}
		i++;
	}
	/*i=0;
	while(i<(result.size())){
		chr = (result[i] & 0xF0) / 0x10;
		if(chr < 10){
			cout << static_cast<char>(chr+48);
		}else{
			cout << static_cast<char>(chr+55); //65 = A; chr + 55 = chr - 10 + 65;
		}
		chr = result[i] & 0x0F;
		if(chr < 10){
			cout << static_cast<char>(chr+48);
		}else{
			cout << static_cast<char>(chr+55); //65 = A; chr + 55 = chr - 10 + 65;
		}
		cout << ' ';
		i++;
	}*/
	cout << "Compiled code sucessfully.";
	ofstream outputFile(string(argv[1])+".portc", ios::binary);
    if (outputFile.is_open()) {
        outputFile.write(result.data(), result.size());
    }
	outputFile.close();
	return 0;
}