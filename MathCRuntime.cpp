#include <vector>
#include <iostream>
#include <stack>
#include <fstream>
#include <chrono>
#include <Windows.h>
using namespace std;
stack<unsigned int> myStack;
unsigned int i = 0;
unsigned char nested = 0;
unsigned short result = 0;
unsigned char chr = 0;
unsigned short firstOperand;
unsigned short secondOperand;
unsigned int numberOfTabs;
bool isNegative;
bool tempBool;
bool tempBool2;
bool isInDebugMode;
unsigned short TEMP;
unsigned short TEMP2;
unsigned short TEMP3;
vector<unsigned char>::iterator TEMP4;
vector<unsigned char> bytecode;
vector<unsigned int> functionPointers;
struct myStruct{
	unsigned char flags;
	unsigned short address;
};
vector<myStruct> varInfo;
vector<unsigned char> varAlloc;
unsigned int boolOrInt[8];
string myString;
HINSTANCE hDll;
typedef void (*myFunctionType)(const vector<unsigned char>&, unsigned char);
myFunctionType myFunction;
int identNumberOfTabs(){
	cout << '\n';
	unsigned int j = 0;
	while(j < numberOfTabs){
		cout << '\t';
		j++;
	}
}
int exitWithError(string error){
	cout << "\033[1;31m(X) Runtime Error: " + error << "\033[0m";
	exit(EXIT_FAILURE);
}
void evalExp(){
	unsigned int temporary;
	if(bytecode[i] == 0xF7){
		i++;
		result = bytecode[i];
		isNegative = 0;
	}else if(bytecode[i] == 0xFF){
		i++;
		chr = varInfo[bytecode[i]].flags&0x10;
		isNegative = !(varInfo[bytecode[i]].flags&1);
		temporary = varInfo[bytecode[i]].address;
		result = varAlloc[temporary];
		if(chr){
			//cout << static_cast<unsigned int>(varAlloc[varInfo[(bytecode[i]*2)+1]]) << '|' << static_cast<unsigned int>(varAlloc[varInfo[(bytecode[i]*2)+1]+1]) << '\n';
			temporary++;
			isNegative = ((varAlloc[temporary]&0x80) != 0) && isNegative;
			result = result + (varAlloc[temporary] * 256);
		}else{
			isNegative = (((result&0x80) != 0) && isNegative);
		}
	}else if(bytecode[i] == 0b11100101){
		i++;
		temporary = varInfo[bytecode[i]].address;
		chr = bytecode[i];
		i++;
		evalExp();
		if(varInfo[chr].flags&0x10){
			result = result * 2;
		}
		temporary = temporary + result;
		if(chr == (varInfo.size()-1)){
			if(result>=(varInfo.size())){
				exitWithError("Operand in . bigger than allocated memory");
			}
		}else{
			if(result>=(varInfo[chr+1].address)){
				exitWithError("Operand in . bigger than size of variable");
			}
		}
		result = varAlloc[temporary];
		if(varInfo[chr].flags&0x10){
			temporary++;
			result = result + varAlloc[temporary]*256;
		}
	}else if(bytecode[i] == 0b11101011){
		i++;
		temporary = ((varInfo[bytecode[i]].flags&0x10)/0x10)+1;
		if(static_cast<size_t>(bytecode[i])==static_cast<size_t>(varInfo.size()-1)){
			result = (varAlloc.size()-varInfo[varInfo.size()-1].address)/temporary;
		}else{
			result = (varInfo[bytecode[i]+1].address-varInfo[bytecode[i]].address)/temporary;
		}
	}else{
		bool sign1;
		unsigned char operation = bytecode[i];
		i++;
		evalExp();
		tempBool = isNegative;
		temporary = result;
		i++;
		evalExp();
		switch(operation){
			case 0b10100000:
				if(tempBool != isNegative){
					result = 0;
				}else{
					if(temporary == result){
						result = 1;
						break;
					}
					result = 0;
				}
				break;
			case 0b10100001: //TEMP1   >TEMP2
			                 //unsigned negative
				if((!tempBool)&&isNegative){
					result = 1;
				}else if(tempBool&&(!isNegative)){
					result = 0;
				}else{
					result = (temporary > result);
				}
				break;
			case 0b10100010:
				//cout << tempBool << ' ' << isNegative << '_' << temporary << ' ' << result;
				if(tempBool&&(!isNegative)){
					result = 1;
				}else if((!tempBool)&&isNegative){
					result = 0;
				}else{
					result = (temporary < result);
				}
				break;
			case 0b10100011:
				result = temporary + result;
				break;
			case 0b10100100:
				result = temporary - result;
				break;
			case 0b10100110:
				result = temporary * result;
				break;
			case 0b10100111:
				result = temporary / result;
				break;
			case 0b10101000:
				if(isNegative){
					result = 0-result;
				}
				result = temporary % result;
				if(isNegative){
					result = 0-result;
				}
				break;
			case 0b10101001:
				result = temporary & result;
				break;
			case 0b10101010:
				result = temporary | result;
				break;
		}
	}
}
int main(int argc, char* argv[]){

    // Open the file in binary mode
    ifstream file(argv[1], ios::binary);

    // Check if the file is open
    if (file.is_open()) {
        file.seekg(0, ios::end);
        streampos fileSize = file.tellg();
        file.seekg(0, ios::beg);
		bytecode.resize(static_cast<size_t>(fileSize));
        // Read the file contents into the vector
        file.read(reinterpret_cast<char*>(bytecode.data()), fileSize);
        // Close the file
        file.close();
    }
    auto start_time = std::chrono::high_resolution_clock::now();

	while(i < bytecode.size()){
		if(bytecode[i]&128){
			switch(bytecode[i]){
				case 0b11000000: //call
					i++;
					myStack.push(i);
					i = functionPointers[bytecode[i]];
					if(isInDebugMode){
						identNumberOfTabs();
						cout << "fn{";
					}
					break;
				case 0b11000001: //break
					i++;
					while(bytecode[i]!=0b11111011){
						i++;
					}
					if(isInDebugMode){
						identNumberOfTabs();
						cout << "break";
						numberOfTabs--;
						identNumberOfTabs();
						cout << '}';
					}
					break;
				case 0b11000010: //if
					i++;
					evalExp();
					if(isInDebugMode){
						cin.ignore();
						identNumberOfTabs();
						cout << "if(" << result << "){";
						if(result!=0){
							numberOfTabs++;
						}
					}
					if(result==0){
						if(isInDebugMode){
							identNumberOfTabs();
							cout << '}';
						}
						i++;
						nested = 0;
						tempBool = 1;
						while(true){
							if(tempBool&&(bytecode[i]==0b11001100)){
								if(nested==0){
									break;
								}
								nested--;
							}else if(bytecode[i]==0b11000010){
								nested++;
							}else if((bytecode[i]==0b11001011)||(bytecode[i]==0b11001101)||(bytecode[i]==0b11001111)){
								if(tempBool){
									tempBool = 0;
								}
							}else if((bytecode[i]==0b11000110)||(bytecode[i]==0b11000101)||(bytecode[i]==0b11111111)||(bytecode[i]==0b11110111)){
								i++;
							}else if(bytecode[i]==0){
								if(tempBool == 0){
									tempBool = 1;
								}
							}
							i++;
						}
					}
					break;
				case 0b11000011: //while
					i--;
					myStack.push(i);
					i++;
					i++;
					evalExp();
					if(isInDebugMode){
						cin.ignore();
						identNumberOfTabs();
						cout << "while(" << result << "){";
						if(result!=0){
							numberOfTabs++;
						}
					}
					if(result==0){
						if(isInDebugMode){
							identNumberOfTabs();
							cout << '}';
						}
						i++;
						nested = 0;
						tempBool = 1;
						while(true){
							if(bytecode[i]==0b11100000){
								if((nested==0)&&tempBool){
									//cout << '-' << i;
									break;
								}
								nested--;
							}else if((bytecode[i]==0b11001011)||(bytecode[i]==0b11001101)||(bytecode[i]==0b11001111)){
								if(tempBool){
									tempBool = 0;
								}
							}else if((bytecode[i]==0b11000110)||(bytecode[i]==0b11000101)||(bytecode[i]==0b11111111)||(bytecode[i]==0b11110111)){
								i++;
							}else if(bytecode[i]==0){
								if(tempBool == 0){
									tempBool = 1;
								}
							}
							i++;
						}
						myStack.pop();
					}
					break;
				case 0b11000100: //fn
					functionPointers.push_back(i);
					i++;
					tempBool = 1;
					nested = 0;
					while(1){
						if(bytecode[i]==0b11000011){
							nested++;
						}else if(bytecode[i]==0b11100000){
							if((nested==0)&&tempBool){
								//cout << '-' << i;
								break;
							}
							nested--;
						}else if((bytecode[i]==0b11001011)||(bytecode[i]==0b11001101)||(bytecode[i]==0b11001111)){
							if(tempBool){
								tempBool = 0;
							}
						}else if((bytecode[i]==0b11000110)||(bytecode[i]==0b11000101)||(bytecode[i]==0b11111111)||(bytecode[i]==0b11110111)){
							i++;
						}else if(bytecode[i]==0){
							if(tempBool == 0){
								tempBool = 1;
							}
						}
						i++;
					}
					break;
				case 0b11000101: //variable = value
					i++;
					//cout << "Variable #" << static_cast<unsigned int>(bytecode[i]) << ' ';
					TEMP2 = bytecode[i];
					i++;
					evalExp();
					TEMP3 = varInfo[TEMP2].address;
					//cout << " equals to " << result << '\n';
					varAlloc[TEMP3] = result;
					//debugMessage("VARIABLE " + to_string(TEMP2) + " CHANGED TO " + to_string(result), false);
					if(varInfo[TEMP2].flags&0x10){
						TEMP3++;
						varAlloc[TEMP3] = (result/256);
					}
					if(isInDebugMode){
						identNumberOfTabs();
						cout << "$" << TEMP2 << " = " << result;
					}
					//cout << static_cast<unsigned int>((result&0xFF00)/0x100) << '|' << static_cast<unsigned int>(result & 0x00FF) << '\n';
					break;
				case 0b11000110: //variable[i] = value / .INTEGERvariable = value
					i++;
					TEMP = bytecode[i];
					if(isInDebugMode){
						identNumberOfTabs();
						cout << '$' << TEMP << '.';
					}
					i++;
					evalExp();
					TEMP2 = result+(varInfo[TEMP].address);
					cout << TEMP2;
					if(TEMP == (varInfo.size()-1)){
						TEMP3 = varInfo.size();
					}else{
						TEMP3 = varInfo[TEMP+1].address;
					}
					//cout << (varInfo[bytecode[i-1]+1].address - 1) << ' ';
					if(TEMP2>=TEMP3){
						exitWithError("Buffer overflow error!");
					}else{
						varAlloc.erase(varAlloc.begin() + static_cast<size_t>(TEMP));
					}
					if(varInfo[TEMP].flags&0x10){
						result = result*2;
					}
					i++;
					evalExp();
					varAlloc[TEMP2] = result;
					if(varInfo[TEMP].flags&0x10){
						TEMP2++;
						varAlloc[TEMP2] = result/256;
					}
					if(isInDebugMode){
						cout << " = " << result;
					}
					//cout << static_cast<unsigned int>(varAlloc[TEMP2]);
					break;
				case 0b11000111: //print
					i++;
					TEMP3 = varInfo[bytecode[i]].address; //Where it gets the address
					result = varAlloc[TEMP3];
					if(varInfo[bytecode[i]].flags&0x10){
						TEMP3++;
						result += varAlloc[TEMP3]*256;
					}
					cout << '\n';
					if(isInDebugMode){
						identNumberOfTabs();
						cout << "print $" << static_cast<unsigned int>(bytecode[i]) << " = ";
					}
					cout << result;
					break;
				case 0b11001000: //delete
					i=i+2;
					TEMP = (varInfo[bytecode[i-1]].address + bytecode[i]);
					if(varInfo[bytecode[i-1]].flags&0x10){
						TEMP = TEMP * 2;
					}
					if(bytecode[i-1] == (varInfo.size()-1)){
						TEMP2 = varInfo.size();
					}else{
						TEMP2 = varInfo[bytecode[i-1]+1].address;
					}
					//cout << (varInfo[bytecode[i-1]+1].address - 1) << ' ';
					if(TEMP>=TEMP2){
						exitWithError("Buffer overflow error!");
					}else{
						varAlloc.erase(varAlloc.begin() + static_cast<size_t>(TEMP));
					}
					TEMP2 = bytecode[i-1];
					TEMP2++;
					while(static_cast<size_t>(TEMP2) < varInfo.size()){
						//cout << TEMP2;
						if(TEMP==2){
							varInfo[TEMP2].address--;
						}
						varInfo[TEMP2].address--;
						TEMP2++;
					}
					if(isInDebugMode){
						identNumberOfTabs();
						cout << "delete $"+bytecode[i-1]+' '+bytecode[i];
					}
					break;
				case 0b11001001: //push
					i=i+2;
					TEMP = ((varInfo[bytecode[i-1]].flags&0x10)/0x10)+1;
					if(static_cast<size_t>(bytecode[i-1])==static_cast<size_t>(varInfo.size()-1)){
						varAlloc.resize(varAlloc.size()+static_cast<size_t>(bytecode[i]*TEMP));
					}else{
						//cout << (varInfo[bytecode[i-1]+1].address - 1) << ' ';
						varAlloc.insert(varAlloc.begin() + static_cast<size_t>(varInfo[bytecode[i-1]+1].address - 1), static_cast<size_t>(bytecode[i] * TEMP), 0);
						TEMP2 = bytecode[i-1];
						TEMP2++;
						while(static_cast<size_t>(TEMP2) < varInfo.size()){
							//cout << TEMP2;
							varInfo[TEMP2].address = varInfo[TEMP2].address + (bytecode[i]*TEMP);
							TEMP2++;
						}
					}
					if(isInDebugMode){
						identNumberOfTabs();
						cout << "push $"+bytecode[i-1]+' '+bytecode[i];
					}
					break;
				case 0b11001011: {//load Windows.dll
					i++;
					myString = "";
					while(bytecode[i]!=1){
						myString.push_back(bytecode[i]);
						i++;
					}
					hDll = LoadLibrary(myString.c_str());
					myString = "";
					i++;
					while(bytecode[i]!='\0'){
						myString.push_back(bytecode[i]);
						i++;
					}
					myFunction = reinterpret_cast<myFunctionType>(GetProcAddress(hDll, myString.c_str()));
					i++;
					if(static_cast<size_t>(bytecode[i]) == (varInfo.size()-1)){
						TEMP4 = varAlloc.begin()+varInfo[bytecode[i]+1].address;
					}else{
						TEMP4 = varAlloc.end();
					}
					vector<unsigned char> subVector(varAlloc.begin()+varInfo[bytecode[i]].address, TEMP4);
					myFunction(subVector, (varInfo[bytecode[i]]).flags);
					break;
				}
				case 0b11100000: //the } terminatings while loops and functions
					if(isInDebugMode){
						numberOfTabs--;
						identNumberOfTabs();
						cout << '}';
					}
					i = myStack.top();
					myStack.pop();
					break;
				case 0b11001100:
					if(isInDebugMode){
						numberOfTabs--;
						identNumberOfTabs();
						cout << '}';
					}
					break;
				case 0b11001101:
					i++;
					while(bytecode[i]!='\0'){
						cout << bytecode[i];
						i++;
					}
					break;
				case 0b11001010:
					isInDebugMode = 1;
					break;
				case 0b11001111: //error
					i++;
					cout << "\n\033[1;31m";
					while(bytecode[i]!='\0'){
						cout << bytecode[i];
						i++;
					}
					cout << "\033[0m";
					exit(EXIT_FAILURE);
					break;
				case 0b11010000: //cascade
					if(isInDebugMode){
						identNumberOfTabs();
						numberOfTabs++;
						cout << "cascade{";
					}
					nested = 0;
					while(true){
						i++;
						evalExp();
						if(result == 0){
							i++;
							nested=0;
							tempBool = 1;
							while(true){
								if(bytecode[i]==0b11010011){
									if((nested==0)&&tempBool){
										break;
									}
									nested--;
								}else if(bytecode[i]==0b11010000){
									nested++;
								}else if(bytecode[i]==0b11010010){
									if(nested==0){
										break;
									}
								}else if((bytecode[i]==0b11001011)||(bytecode[i]==0b11001101)||(bytecode[i]==0b11001111)){
									if(tempBool){
										tempBool = 0;
									}
								}else if((bytecode[i]==0b11000110)||(bytecode[i]==0b11000101)||(bytecode[i]==0b11111111)||(bytecode[i]==0b11110111)){
									i++;
								}else if(bytecode[i]==0){
									if(tempBool == 0){
										tempBool = 1;
									}
								}
								i++;
							}
						}else{
							break;
						}
					}
					break;
				case 0b11010011:
					tempBool = 1;
					while(true){
						if(bytecode[i]==0b11010010){
							if((nested==0)&&tempBool){
								break;
							}
							nested--;
						}else if(bytecode[i]==0b11010000){
							nested++;
						}else if((bytecode[i]==0b11001011)||(bytecode[i]==0b11001101)||(bytecode[i]==0b11001111)){
							if(tempBool){
								tempBool = 0;
							}
						}else if((bytecode[i]==0b11000110)||(bytecode[i]==0b11000101)||(bytecode[i]==0b11111111)||(bytecode[i]==0b11110111)){
							i++;
						}else if(bytecode[i]==0){
							if(tempBool == 0){
								tempBool = 1;
							}
						}
						i++;
					}
					break;
			}
		}else{
			myStruct TEMPstruct;
			chr = bytecode[i];
			TEMPstruct.flags = chr;
			TEMPstruct.address = varAlloc.size();
			varInfo.push_back(TEMPstruct);
			if(bytecode[i] & 2){
				i++;
				TEMP = bytecode[i];
				if(bytecode[i] & 8){
					i++;
					TEMP = TEMP + bytecode[i];
				}
			}else{
				TEMP = 1;
			}
			if(chr&0x10){
				chr = 2;
			}else{
				chr = 1;
			}
			varAlloc.resize(varAlloc.size()+(TEMP*chr));
			//cout << "SIZE" << varAlloc.size();
			//cout << 'S' << varAlloc.size();
		}
		i++;
	}
    // Record the end time
    auto end_time = chrono::high_resolution_clock::now();

    // Calculate the duration
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    // Print the duration
    cout << "\nEnded in " << duration.count() << " microseconds";
	return 0;
}