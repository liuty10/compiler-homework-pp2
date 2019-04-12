/* This is main file for decaf compiler: dcc
 * Author: Tianyi Liu
 * Email: liuty10@gmail.com
*/

/*
 Goal : Design a PARSER to check syntax errors and generate AST..
 Task1: read tokens from lexer's output file.
 Task2: for each line, there is only one token.
 Task3: Deal with each token
        1)create a PROGRAM object at the beginning.
           class program{ //it is a list of declarations
               
           }

        2)read token, and create FncDecl object or VarDecl object.
        3)
*/

/*
 How to use dcc (decaf gcc)?
 Goal: it should be simallar to gcc.
 Example: dcc -t input -o [output]

*/

#include "define.h"
#include <stdio.h>
class Declaration{
    public:
	int getType(){
            return retType;
        }
        void setType(int Type){
            retType = Type;
        }
        char* getIdentifier(){
            return Identifier;
        }
        void setIdentifier(char* idenAddr){
            int i;
            for(i=0;i<strlen(idenAddr);i++)
                Identifier[i] = idenAddr[i];
        }

    protected:
        int DeclSeqID;
        int retType;
        char Identifier[256];
};

class VarDecl: public Declaration{
};
/*
//TODO: define each Expr
union{
    struct{

    }whileStmt* assignmentHead;

    struct{

    }IfStmt* assignmentHead;

    struct{

    }forStmt* assignmentHead;

    struct{

    }ReturnStmt* assignmentHead;

    struct{

    }BreakStmt* assignmentHead;

    struct{

    }AssignExpr* assignmentHead;

    struct{

    }ArithmeticExpr* assignmentHead;

    struct{

    }LogicalExpr* assignmentHead;
}

class bodyStmt{
    int stmtCategory;
    int stmtSeqNum;
    bodyStmt* last;
    bodyStmt* cur;
    bodyStmt* next;
}

class FncDecl: public Declaration{
    protected:
	VarDecl* formalHead;
        bodyStmt* bodyHead;
        
        int      formalTotal;
        int      bodyTotal;
}
*/
class Program{
    public:
        Program(){
            head = NULL;
            DeclTotal = 0;
        };
        void print(char* str){
            printf("%s\n", str);
        };
    protected:
        Declaration  	*head;
        int		DeclTotal;
};

int main(){
    char szLineBuffer[MAX_LINE_SIZE + 1];//input buffer for fgets
    Program *prog = new Program();
    FILE *token_file   = fopen("./token_file.txt", "r");
    while(fgets(szLineBuffer, MAX_LINE_SIZE, token_file)!=NULL){
    	//row_num++;
    	if(szLineBuffer[0] == '\n') continue;
    	if(buildAST(szLineBuffer, row_num, prog)== -1){
    		break;
    	}
        prog->print(szLineBuffer);
    }
    //prog.print();
    fclose(token_file);
    delete prog;
    return 0;
}
