#include "define.h"
bool isDelimiter(char ch){
	if (ch == ' ' || ch == '+' || ch == '-' || ch == '*' ||
	    ch == '/' || ch == ',' || ch == ';' || ch == '>' ||
	    ch == '<' || ch == '=' || ch == '(' || ch == ')' ||
	    ch == '[' || ch == ']' || ch == '{' || ch == '}' ||
	    ch == '!' || ch == '@' || ch == '#' || ch == '$' ||
	    ch == '^' || ch == '?' || ch == '|' || ch == '~' ||
	    ch == '.' || ch == '"' || ch == ':' || ch == '\n'||
	    ch == '&' || ch == '%' || ch == '`')
		return true;
	return false;
}

void print_errors(int num_err, char* bufferStr, int row){
	switch(num_err){
		case ERR_Unterminated:
			printf("\n*** Error line %d.\n", row);
			printf("*** Unterminated string constant: %s\n\n", bufferStr);
			break;
		case ERR_TooLongVariable:
			printf("\n*** Error line %d.\n", row);
			printf("*** Inditifier too long: \"%s\".\n\n", bufferStr);
			break;
		case ERR_UnknownSymbol:
			printf("\n*** Error line %d.\n", row);
			printf("*** Unterminated recognized symbol: %s\n\n", bufferStr);
			break;
		case ERR_InvalidDirective:
			printf("\n*** Error line %d.\n", row);
			printf("*** Invalid # directive\n\n");
			break;
		default:
			break;
	}
	//printf("There is an error\n");
	return;
}
