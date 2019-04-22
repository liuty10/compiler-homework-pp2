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
#define T_NULL           0
#define T_BoolConstant   1
#define T_IntConstant    2
#define T_DoubleConstant 3
#define T_StringConstant 4
#define T_Identifier     5

#define T_Void          6
#define T_Int		7
#define T_Double	8
#define T_Bool		9
#define T_String 	10
#define T_Null	 	11
#define T_For		12
#define T_While		13
#define T_If		14
#define T_Else		15
#define T_Return	16
#define T_Break		17
#define T_Print		18
#define T_ReadInteger	19
#define T_ReadLine	20

#define T_Assign	21
#define T_Logic_Or	22
#define T_Logic_And	23
#define T_Equal		24
#define T_NotEqual	25
#define T_Less		26
#define T_LessEqual	27
#define T_Larger	28
#define T_GreaterEqual	29
#define T_Add		30
#define T_Sub		31
#define T_Mul		32
#define T_Div		33
#define T_Percent	34
#define T_Logic_Not	35

#define T_Bitwise_Or    36
#define T_Bitwise_And	37
#define T_Bitwise_Not	38

#define T_Point		39
#define T_Increase	40
#define T_Decrease	41
#define T_LPara		42
#define T_RPara		43
#define T_LCurvePara	44
#define T_RCurvePara	45
#define T_LSquarePara	46
#define T_RSquarePara	47

#define T_Comma		48
#define T_SemiColon	49

#define T_Others	50
#define T_Unknown	51
#define T_Comment_Line		52
#define T_Comment_Multiple	53

//Define Error types
#define ERR_NULL		0
#define ERR_Unterminated        1
#define ERR_TooLongVariable  	2
#define ERR_UnknownSymbol  	3
#define ERR_InvalidDirective  	4
#define ERR_Others              5

//Decl
#define DECL_VAR       1
#define DECL_FUNC      2

//Parser Status
#define STATUS_PROGRAM      1
#define STATUS_DECL         2
#define STATUS_FORMAL       3
#define STATUS_FUNCSTART    4
#define STATUS_FUNCBODY     5

//Parser: stmt category
#define STMT_VAR     0
#define STMT_BLOCK     1
#define STMT_IF        2
#define STMT_FOR       3
#define STMT_WHILE     4
#define STMT_RET       5
#define STMT_BREAK     6
#define STMT_PRINT     7
#define STMT_EXPR      8
#define STMT_CALL      100

#define EXPR_ASSIGN    9
#define EXPR_CONSTANT  10
#define EXPR_IDENT     11
#define EXPR_CALL      12
#define EXPR_PARA      13//(Expr)
#define EXPR_ARITHM    14
#define EXPR_EQUAL     15
#define EXPR_RELATION  16
#define EXPR_LOGIC     17
#define EXPR_READINT   18
#define EXPR_READLINE  19
#define EXPR_INTCONST     20
#define EXPR_DOUBLECONST  21
#define EXPR_BOOLCONST    22
#define EXPR_STRINGCONST  23
#define EXPR_NULLCONST    24

struct token{
    int row;
    int left;
    int right;
    int category;
    int err_num;
    char token[MAX_TOKEN_SIZE+1];
};
