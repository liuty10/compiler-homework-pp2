/* This is main file for decaf compiler: dcc
 * Author: Tianyi Liu
 * Email: liuty10@gmail.com
*/

/*
 Goal : Design a scanner to deal with decaf source code into TOKON.
 Task1: read source code from files line by line.
 Task2: for each line, read characters one by one to split the whole line into pre-TOKONS.
 Task3: For each pre-token, you need further process to get the final TOKON.
 Task4: Write the corresponding tokon back to a result file.
*/

/*
 How to use dcc (decaf gcc)?
 Goal: it should be simallar to gcc.
 Example: dcc -t input -o [output]

*/

#include"define.h"
extern bool isDelimiter(char ch);
extern void print_errors(int num_err, char* bufferStr, int row);

int  possible_category = 0;
int  deterministic_category = 0;
bool hex_flag = false;
bool science_flag = false;
bool space_flag = false;
int  err_num = 0;
struct token tokenInRow[MAX_TOKEN_IN_ROW];

bool newTokenEnd(char* tokenBuffer, char *ch, int *pleft, int *pright, int len){
	deterministic_category = T_NULL;
	if(possible_category == T_NULL){
		hex_flag = false;
		science_flag = false;
		space_flag = false;
		if(*ch >= '0' && *ch <='9')
			possible_category = T_IntConstant;
		else if((*ch >= 'A' && *ch <= 'Z') || (*ch>='a' && *ch<='z') || *ch == '_')
			possible_category = T_Identifier;
		else if(isDelimiter(*ch)==true){	//we stop here
			if(*ch == '!' || *ch == '>' || *ch == '<' || *ch == '='){
				if(*pright==len || *(ch+1)!='='){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Others;
                                        if(*ch == '!') deterministic_category = T_Logic_Not;
                                        if(*ch == '>') deterministic_category = T_Larger;
                                        if(*ch == '<') deterministic_category = T_Less;
                                        if(*ch == '=') deterministic_category = T_Assign;
				}else{
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='=';
					tokenBuffer[2]='\0';
					(*pright)++;
					if(*ch == '!') deterministic_category = T_NotEqual;
					if(*ch == '>') deterministic_category = T_GreaterEqual;
					if(*ch == '<') deterministic_category = T_LessEqual;
					if(*ch == '=') deterministic_category = T_Equal;
				}
				possible_category = T_NULL;
				return true;
			}else if(*ch == '.'){
				tokenBuffer[0]=*ch;
				tokenBuffer[1]='\0';
				deterministic_category = T_Point;
				//deterministic_category = T_Others;
				possible_category = T_NULL;
				return true;
			}else if(*ch == '"'){// continuing until the end of line or next quote. Or, we need T_String
				possible_category = T_StringConstant;
				return false;
			}else if(*ch == ' '){
				(*pleft)++;
				(*pright)++;
				possible_category = T_NULL;
				space_flag = true;
				return false;
			}else if(*ch == '\n'){
				return false;
			}else if(*ch=='|'){
				if((*pright)==len || *(ch+1)!='|'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Bitwise_Or;
					possible_category = T_NULL;
					return true;
				}
				if(*(ch+1) == '|'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]=*ch;
					tokenBuffer[2]='\0';
					deterministic_category = T_Logic_Or;
					possible_category = T_NULL;
					(*pright)++;
					return true;
				}else{
					print_errors(ERR_Others, tokenBuffer, 0);
				}
				
			}else if(*ch=='&'){
				if((*pright)==len || *(ch+1)!='&'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Bitwise_And;
					possible_category = T_NULL;
					return true;
				}
				if(*(ch+1) == '&'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]=*ch;
					tokenBuffer[2]='\0';
					deterministic_category = T_Logic_And;
					possible_category = T_NULL;
					(*pleft)++;
					return true;
				}else{
					print_errors(ERR_Others, tokenBuffer,0);
				}
			}else if(*ch=='~'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Bitwise_Not;
					possible_category = T_NULL;
					return true;
			}else if(*ch=='('){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_LPara;
					possible_category = T_NULL;
					return true;
			}else if(*ch==')'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_RPara;
					possible_category = T_NULL;
					return true;
			}else if(*ch=='{'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_LCurvePara;
					possible_category = T_NULL;
					return true;
			}else if(*ch=='}'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_RCurvePara;
					possible_category = T_NULL;
					return true;
			}else if(*ch=='['){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_LSquarePara;
					possible_category = T_NULL;
					return true;
			}else if(*ch==']'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_RSquarePara;
					possible_category = T_NULL;
					return true;
			}else if(*ch==','){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Comma;
					possible_category = T_NULL;
					return true;
			}else if(*ch==';'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_SemiColon;
					possible_category = T_NULL;
					return true;
			}else if(*ch=='%'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Percent;
					possible_category = T_NULL;
					return true;
			}else if(*ch=='*'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Mul;
					possible_category = T_NULL;
					return true;
			}else if(*ch=='+'){
				if((*pright)==len || *(ch+1)!='+'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Add;
					possible_category = T_NULL;
					return true;
				}
				if(*(ch+1) == '+'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]=*ch;
					tokenBuffer[2]='\0';
					deterministic_category = T_Increase;
					possible_category = T_NULL;
					(*pleft)++;
					return true;
				}else{
					print_errors(ERR_Others, tokenBuffer,0);
				}
			}else if(*ch=='-'){
				if((*pright)==len || *(ch+1)!='-'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Sub;
					possible_category = T_NULL;
					return true;
				}
				if(*(ch+1) == '-'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]=*ch;
					tokenBuffer[2]='\0';
					deterministic_category = T_Decrease;
					possible_category = T_NULL;
					(*pleft)++;
					return true;
				}else{
					print_errors(ERR_Others, tokenBuffer,0);
				}
			}else if(*ch=='/'){
				if((*pright)<len){
					if(*(ch+1)=='/'){
						possible_category = T_Comment_Line;
						return false;
					}else if(*(ch+1)=='*'){
						possible_category = T_Comment_Multiple;
						(*pright)++;
						return false;
					}else{
						tokenBuffer[0]=*ch;
						tokenBuffer[1]='\0';
						deterministic_category = T_Div;
						possible_category = T_NULL;
						return true;
					}
				}else{
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Others;
					possible_category = T_NULL;
					return true;
				}
				if((*pright)==len || *(ch+1)!='&'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]='\0';
					deterministic_category = T_Bitwise_And;
					possible_category = T_NULL;
					return true;
				}
				if(*(ch+1) == '&'){
					tokenBuffer[0]=*ch;
					tokenBuffer[1]=*ch;
					tokenBuffer[2]='\0';
					deterministic_category = T_Logic_And;
					possible_category = T_NULL;
					(*pleft)++;
					return true;
				}else{
					print_errors(ERR_Others, tokenBuffer,0);
				}
			}else{
				tokenBuffer[0]=*ch;
				tokenBuffer[1]='\0';
				deterministic_category = T_Others;
				possible_category = T_NULL;
				return true;
			}
		}else{
			//TODO: T_Unknown
			possible_category = T_NULL;
			tokenBuffer[0]=*ch;
			tokenBuffer[1]='\0';
			deterministic_category = T_Unknown;
			err_num = ERR_UnknownSymbol;
			//print_errors();
			return true;
		}
		return false;	
	}
	if(possible_category == T_IntConstant){
		if(*ch >= '0' && *ch <='9'){
			return false;
		}else if((*ch >= 'A' && *ch <= 'Z') || (*ch>='a' && *ch<='z') || *ch == '_'){
			if(hex_flag == false && (*ch == 'x' || *ch == 'X')&&(*(ch-1) == '0')){
				hex_flag = true;//do not change category yet.
				return false;
			}else if(hex_flag == true && ((*ch >='a' && *ch <= 'f')||(*ch >='A' && *ch <='F')||(*ch>='0' && *ch <='9'))){
				return false;//DO not change category
			}else{//need to change category
				if(hex_flag==true && (*(ch-1)=='x' || *(ch-1)=='X')){
					tokenBuffer[(*pright)-(*pleft)-1]='\0';
					(*pright)=(*pright) - 2;
				}else{
					tokenBuffer[(*pright)-(*pleft)]='\0';
					(*pright)--;
				}
				deterministic_category = T_IntConstant;
				possible_category = T_NULL;
				return true;
			}
		}else if(isDelimiter(*ch)==true){	//we stop here
			if(*ch == '.'){
				tokenBuffer[(*pright)-(*pleft)]=*ch;
				possible_category = T_DoubleConstant;
				return false;
			}else if(*ch == '"'){// continuing until the end of line or next quote. Or, we need T_String
				tokenBuffer[(*pright)-(*pleft)] = '\0';
				(*pright)--;
				deterministic_category = T_IntConstant;
				possible_category = T_NULL;
				return true;
			}else{
				if(hex_flag==true && (*(ch-1)=='x' || *(ch-1)=='X')){
					tokenBuffer[(*pright) - (*pleft)-1]='\0';
					(*pright)-=2;
				}else{
					tokenBuffer[(*pright) - (*pleft)]='\0';
					//if(*ch != ' '&& *ch !='\n')
					(*pright)--;
				}
				deterministic_category = T_IntConstant;
				possible_category = T_NULL;
				return true;
			}
		}else{
			//print_errors();
			err_num = ERR_UnknownSymbol;
			tokenBuffer[(*pright)-(*pleft)]='\0';
			(*pright)--;
			deterministic_category = T_IntConstant;
			possible_category = T_NULL;
			return true;
		}
		return false;	
	}

	if(possible_category == T_DoubleConstant){
		if(*ch >= '0' && *ch <='9'){
			return false;
		}else if((*ch >= 'A' && *ch <= 'Z')|| (*ch >='a'&&*ch<='z') || *ch == '_'){
			if(science_flag == true){
				//*ch-1 is e or E, back 1 split
				if(*(ch-1) == 'e' || *(ch-1) == 'E'){
					tokenBuffer[(*pright)-(*pleft)-1]='\0';
					(*pright) = (*pright) - 2;
					deterministic_category = T_DoubleConstant;
					possible_category = T_NULL;
					return true;//double
				}
				//*ch-1 is + or -, back 2 split
				if(*(ch-1) == '+' || *(ch-1) == '-'){
					tokenBuffer[(*pright)-(*pleft)-2]='\0';
					(*pright) = (*pright) - 3;
					deterministic_category = T_DoubleConstant;
					possible_category = T_NULL;
					return true;//double
				}
				//*ch-1 is digit,  split
				if(*(ch-1) >= '0' && *(ch-1) <= '9'){
					tokenBuffer[(*pright)-(*pleft)]='\0';
					(*pright) = (*pright) - 1;
					deterministic_category = T_DoubleConstant;
					possible_category = T_NULL;
					return true;//double
				}
			}else if(*ch == 'e' || *ch == 'E'){
				science_flag = true;//do not change category yet.
				return false;
			}else{//need to change category
				tokenBuffer[(*pright)-(*pleft)]='\0';
				(*pright)--;
			        deterministic_category = T_DoubleConstant;
				possible_category = T_NULL;
				return true;
			}
		}else if(isDelimiter(*ch)==true){	//we stop here
			if(science_flag == true){
			if((*ch == '+' || *ch == '-') && (*(ch-1) == 'e' || *(ch-1)=='E')){
				tokenBuffer[(*pright)-(*pleft)]=*ch;
				possible_category = T_DoubleConstant;
				return false;
			}else if(*(ch-1)=='e'||*(ch-1)=='E'){// continuing until the end of line or next quote. Or, we need T_String
				tokenBuffer[(*pright)-(*pleft)-1] = '\0';
				*pright = *pright-2;
			        deterministic_category = T_DoubleConstant;
				possible_category = T_NULL;
				return true;
			}else if(*(ch-1)=='+' || *(ch-1)=='-'){
				tokenBuffer[(*pright) - (*pleft) -2]='\0';
				(*pright) = (*pright)-3;
			        deterministic_category = T_DoubleConstant;
				possible_category = T_NULL;
				return true;
			}else{
				tokenBuffer[(*pright)-(*pleft)] = '\0';
				(*pright)--;
			        deterministic_category = T_DoubleConstant;
				possible_category = T_NULL;
				return true;
			}

			}else{
				tokenBuffer[(*pright)-(*pleft)] = '\0';
				(*pright)--;
			        deterministic_category = T_DoubleConstant;
				possible_category = T_NULL;
				return true;
			}
		}else{
			//print_errors();
			err_num = ERR_UnknownSymbol;
			tokenBuffer[(*pright)-(*pleft)]='\0';
			(*pright)--;
			deterministic_category = T_DoubleConstant;
			possible_category = T_NULL;
			return true;
		}
		return false;	

	}

	if(possible_category == T_Identifier){
		if(isDelimiter(*ch)==true){
			tokenBuffer[(*pright)-(*pleft)] = '\0';
			if(strlen(tokenBuffer)>32) err_num=ERR_TooLongVariable;
			(*pright)--;
			deterministic_category = T_Identifier;
			possible_category = T_NULL;
			return true;
		}else{
			tokenBuffer[(*pright) - (*pleft)] = *ch;
			return false;
		}	
	}
	
	if(possible_category == T_StringConstant){
		if(*ch == '"'){
			tokenBuffer[(*pright)-(*pleft)] = *ch;
			tokenBuffer[(*pright)-(*pleft)+1] = '\0';
			deterministic_category = T_StringConstant;
			possible_category = T_NULL;
			return true;
		}else if(*ch=='\n'||(*pright)==len){
			tokenBuffer[(*pright)-(*pleft)] = '\0';
			err_num = ERR_Unterminated;
			//print_errors();
			deterministic_category = T_StringConstant;//need further discuss
			possible_category = T_NULL;
			return true;
		}else
			return false;
	}

	if(possible_category == T_Comment_Multiple){
		if(*ch == '*' && *(ch+1) == '/'){
			deterministic_category = T_Comment_Multiple;
			possible_category = T_NULL;
			return true;
		}else
			return false;
	}

	return false;
}

int check_keyword(char* tokenBuffer){
	if(strcmp(tokenBuffer, "void")==0) 	return T_Void;
	if(strcmp(tokenBuffer, "int")==0) 	return T_Int;
	if(strcmp(tokenBuffer, "double")==0) 	return T_Double;
	if(strcmp(tokenBuffer, "bool")==0) 	return T_Bool;
	if(strcmp(tokenBuffer, "string")==0) 	return T_String;
	if(strcmp(tokenBuffer, "null")==0) 	return T_Null;
	if(strcmp(tokenBuffer, "for")==0) 	return T_For;
	if(strcmp(tokenBuffer, "while")==0) 	return T_While;
	if(strcmp(tokenBuffer, "if")==0) 	return T_If;
	if(strcmp(tokenBuffer, "else")==0) 	return T_Else;
	if(strcmp(tokenBuffer, "return")==0) 	return T_Return;
	if(strcmp(tokenBuffer, "break")==0) 	return T_Break;
	if(strcmp(tokenBuffer, "Print")==0) 	return T_Print;
	if(strcmp(tokenBuffer, "ReadInteger")==0) return T_ReadInteger;
	if(strcmp(tokenBuffer, "ReadLine")==0) 	return T_ReadLine;
	if(strcmp(tokenBuffer, "false")==0) 	return T_BoolConstant;
	if(strcmp(tokenBuffer, "true")==0) 	return T_BoolConstant;
	return T_Identifier;	
}

int check_operator(char* tokenBuffer){
       if(tokenBuffer[0]=='+')         return T_Add;
       if(tokenBuffer[0]=='-')         return T_Sub;
       if(tokenBuffer[0]=='*')         return T_Mul;
       if(tokenBuffer[0]=='/')         return T_Div;
       if(tokenBuffer[0]=='%')         return T_Percent;
       if(tokenBuffer[0]=='<')         return T_Less;
       if(tokenBuffer[0]=='>')         return T_Larger;
       if(tokenBuffer[0]=='!')         return T_Logic_Not;
       if(tokenBuffer[0]=='~')         return T_Bitwise_Not;
       if(tokenBuffer[0]=='=')         return T_Assign;
}


int getTokens(char *inputLine, int cur_row){
	char tokenBuffer[MAX_TOKEN_SIZE + 1];
	int left = 0, right = 0, tokenIndex=0;
	int stringLen = strlen(inputLine);
	
	while(right < stringLen && left <=right){
		if(newTokenEnd(tokenBuffer,&inputLine[right],&left, &right, stringLen-1)){
			if(deterministic_category == T_Comment_Multiple){
			    possible_category = T_NULL;
			    right+=2;
			    left = right;
			    err_num = ERR_NULL;
			    continue;
			}
			//if(err_num!=ERR_NULL && err_num != ERR_TooLongVariable){
		        //    print_errors(err_num, tokenBuffer, cur_row);
			//}else{
			    if(deterministic_category == T_Identifier){
				deterministic_category = check_keyword(tokenBuffer);
			    }
			    //if(deterministic_category == T_Others){
			   //	deterministic_category = check_operator(tokenBuffer);
			    //}
                            tokenInRow[tokenIndex].row   = cur_row;
                            tokenInRow[tokenIndex].left  = left+1;
                            tokenInRow[tokenIndex].right = right+1;
                            tokenInRow[tokenIndex].category = deterministic_category;
                            tokenInRow[tokenIndex].err_num  = err_num;
                            strcpy(tokenInRow[tokenIndex].token, tokenBuffer);//copy
                            tokenIndex++;
                        //}
			right++;
			left = right;
			err_num = ERR_NULL;
		}else{//no new token, we should increase
			if(possible_category == T_Comment_Line){
				possible_category = T_NULL;
				break;
			}
			if(possible_category == T_Comment_Multiple){
				right++;
			}else if(space_flag == false){
				tokenBuffer[right-left]=inputLine[right];
				right++;
			}else{;}
		}
	}
	return tokenIndex;//return the number of tokens that we find in this row	
}

