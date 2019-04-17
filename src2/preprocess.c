/* This is preprocessor, which is used to deal with MACROs in program.
 * Author: Tianyi Liu
 * Email: liuty10@gmail.com
*/

#include "define.h"

extern bool isDelimiter(char ch);
extern void print_errors(int num_err, char* bufferStr, int row);

struct MacroDefination{
        int valid;
	char macro[MAX_LINE_SIZE];
	char value[MAX_LINE_SIZE];
};

struct MacroDefination MacroTable[MACRO_TABLE_SIZE];

void insertMacroTable(char* symbol, char * value){
	int i=0;
	while(i<MACRO_TABLE_SIZE && MacroTable[i].valid == true){
		i++;
	}
	if(i< MACRO_TABLE_SIZE){
		MacroTable[i].valid = true;
		strcpy(MacroTable[i].macro, symbol);
		strcpy(MacroTable[i].value, value);
		
	}else{
		printf("macro table length too small\n");
	}
	
}

bool checkMacroTable(char *symbol){
	int i=0;
	while(i<MACRO_TABLE_SIZE && MacroTable[i].valid == true){
		if(strcmp(symbol, MacroTable[i].macro)==0){
			strcpy(symbol, MacroTable[i].value);
			return true;
		}else{
			i++;
		}
	}
	if(i>= MACRO_TABLE_SIZE){
		symbol[0]='\0';
	}
	return false;

}

void findMacroAndReplace(char* input, char* output, int row){
	int len = strlen(input);
	int i = 0;
	int state = 0;
	int index = 0;
	int left = 0;
	int right = 0;
	char macro_symbol[MAX_LINE_SIZE];
	char macro_value[MAX_LINE_SIZE];
	char *macro_new_line = output;
	while(left<=right && right < len){
		if(state == 0){
			if(input[right]=='\n'){
				macro_new_line[index] = '\0';
				break;
			}
			if(input[right] != '#'){
				macro_new_line[index] = input[right];
				index++;
				right++;
				left = right;
				continue;
			}else{
				right++;
				left = right;
				state = 1;
			}
		}
		if(state == 1){
			if(isDelimiter(input[right])==false){
				macro_symbol[right-left] = input[right];
				right++;
				continue;
			}else{//from left to right-1
				macro_symbol[right-left] = '\0';
				if(strcmp(macro_symbol, "define") == 0){
					right++;
					left = right;
					state = 2;
				}else{
					if(checkMacroTable(macro_symbol)==true){//replace
						int i=0;
						while(macro_symbol[i]!='\0'){
							macro_new_line[index] = macro_symbol[i];
							index++;
							i++;
						}
					}else{
						print_errors(ERR_InvalidDirective,NULL,row);
						macro_new_line[0]='\0';
						break;
					}
					state = 0;
				}
			}
		}
		if(state == 2){
			if(input[right]==' ' && right==left){
				right++;
				left=right;
				continue;
			}else if((input[right]>='0'&&input[right]<='9') ||
				(input[right]>='A'&&input[right]<='Z') || 
				(input[right]>='a'&&input[right]<='z') || 
				input[right] == '_'){
				macro_symbol[right-left] = input[right];
				right++;
			}else if(input[right]==' '){
				macro_symbol[right-left] = '\0';
				right++;
				left = right;
				if(macro_symbol[0]>='0' && macro_symbol[0]<='9'){
					print_errors(ERR_InvalidDirective,NULL,row);
					macro_new_line[0]='\0';
					break;
				}else{
					state = 3;
				}
			}else{
				print_errors(ERR_InvalidDirective,NULL,row);
				macro_new_line[0]='\0';
				break;
			}
		}
		if(state == 3){
			if(input[right]!='\n' && input[right]!='\0'){
				macro_value[right-left] = input[right];
				right++;
			}else{
				macro_value[right-left] = '\0';
				insertMacroTable(macro_symbol, macro_value);
				macro_new_line[0]='\0';
				break;
			}
		}
	}
}

void pre_processor(FILE* source, FILE* dest){
	char szLineBuffer[MAX_LINE_SIZE + 1];   //input buffer for fgets
	char afterLineBuffer[MAX_LINE_SIZE + 1];//output buffer for fputs
	int row = 0;
	while(fgets(szLineBuffer, MAX_LINE_SIZE, source)!=NULL){
		row++;
		if(szLineBuffer[0] == '\n') {
			fputs(szLineBuffer, dest);
		continue;}
		findMacroAndReplace(szLineBuffer, afterLineBuffer, row);
		fputs(afterLineBuffer, dest);
		fputs("\n\0", dest);
	}
	return;
}

