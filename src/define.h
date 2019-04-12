/* This is main file for decaf compiler: dcc
 * Author: Tianyi Liu
 * Email: liuty10@gmail.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX_LINE_SIZE 1000
#define MAX_TOKEN_SIZE 1000
#define MAX_TOKEN_IN_ROW 1000
#define MACRO_TABLE_SIZE 1000
#define true 1
#define false 0
#define bool int

// Define token types
#define T_NULL	 	0
#define T_BoolConstant	1
#define T_IntConstant 	2
#define T_DoubleConstant 3
#define T_StringConstant 4
#define T_Identifier 	25

#define T_Void		5
#define T_Int		6
#define T_Double	7
#define T_Bool		28
#define T_String 	8
#define T_Null	 	9
#define T_For		10
#define T_While		11
#define T_If		12
#define T_Else		13
#define T_Return	14
#define T_Break		15
#define T_Print		16
#define T_ReadInteger	17
#define T_ReadLine	18

#define T_Logic_Or	19
#define T_Logic_And	20
#define T_LessEqual	21
#define T_GreaterEqual	22
#define T_Equal		23
#define T_NotEqual	24

#define T_Others	26
#define T_Unknown	27

#define T_Bitwise_Or		29
#define T_Bitwise_And		30

#define T_Comment_Line		31
#define T_Comment_Multiple	32

//Define Error types
#define ERR_NULL		0
#define ERR_Unterminated	1
#define ERR_TooLongVariable  	2
#define ERR_UnknownSymbol  	3
#define ERR_InvalidDirective  	4
#define ERR_Others	  	5

struct token{
    int row;
    int left;
    int right;
    int category;
    int err_num;
    char token[MAX_TOKEN_SIZE+1];
};
