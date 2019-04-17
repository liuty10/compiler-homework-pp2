/* File: parser.c 
 * --------------
 * This file implements the parser for the grammar.
 */

#include "define.h"

/* Here lists definitions of Expr
 *
*/

//function declarations
extern struct token tokenInRow[MAX_TOKEN_IN_ROW];
extern int getTokens(char *inputLine, int cur_row);
bool parser(FILE* input, FILE* output);
char szLineBuffer[MAX_LINE_SIZE + 1];//input buffer for fgets
int rowTokenNum;
int row_index;


//STMT_EXPR
class Expr{
       public:
             Expr(){

             };
             ~Expr(){};
             void parseExpr();
       public:
             Expr*  left;
             int    leftCategory;
             char   op[2];//two bye to save operators
             Expr*  right;
             int    rightCategory;
             Expr*  next;  // The last two rows are for call(actuals) only,
             int    nextCategory;// since actuals can be a list of Exprs.
};
void Expr::parseExpr(){

};
// STMT_BLOCK
// we need a linked list to save each stmt infor

class bodyStmt{
       public:
            bodyStmt(int categ, void* pointer){
                   category = categ;
                   stmtPointer = pointer;
            };
            ~bodyStmt(){

            }
       public:
       int category;
       void* stmtPointer;//varDecl,if,while,for,break,return,print,expr,
       bodyStmt* next;
};

class VariableDecl{
       public:
           VariableDecl(int declType){
               type = declType;
               ident[0]  = '\0';
               formal = NULL;
           };
           VariableDecl(int declType, char*name){
               type = declType;
               strcpy(ident, name);
               formal = NULL;
           };
           ~VariableDecl(){
               if(formal !=NULL){delete formal; formal = NULL;}
           };
       public:
           int type;//int, double, bool, string
           char ident[MAX_TOKEN_SIZE];//1000
           VariableDecl* formal; //only for functional formals' use
};

class FunctionDecl{
       public:
           FunctionDecl(int declType){
               type = declType;
               ident[0] = '\0';
               formal = NULL;
               stmtFirstInfor   = NULL;
           };
           FunctionDecl(int declType, char* name){
               type = declType;
               strcpy(ident, name);
               formal = NULL;
               stmtFirstInfor   = NULL;
           };
           ~FunctionDecl(){
                if(formal != NULL) {delete formal; formal=NULL;}
                if(stmtFirstInfor   != NULL) {delete stmtFirstInfor; stmtFirstInfor=NULL;}
           };
       VariableDecl* parseFormals(FILE* input_file, int index);
       bodyStmt* parseStmtBlock(FILE* input);
       //bodyStmt* parseStmtBlock(FILE* input, int index);
       public:
           int type;//int, double, bool, string, void
           char ident[MAX_TOKEN_SIZE];//1000
           VariableDecl* formal;//a list of varaibles
           bodyStmt* stmtFirstInfor;
           bodyStmt* stmtCurrentInfor;
};

VariableDecl* FunctionDecl::parseFormals(FILE* input_file,int i){
    int formal_type_start = 0;
    VariableDecl* formalList=NULL;
    VariableDecl* currentFormal=NULL;
    for(;i<rowTokenNum;i++){
        if(formal_type_start ==0){
            if((tokenInRow[i].category==T_Int || tokenInRow[i].category==T_Double ||
                tokenInRow[i].category==T_Bool|| tokenInRow[i].category==T_String)&&formal_type_start ==0){
                formal_type_start = 1;
            }else{
                printf("parse formal error, expect type\n");
                exit(0);
            }
        }else if(formal_type_start == 1){
            if(tokenInRow[i].category==T_Identifier){
                formal_type_start = 2;
                VariableDecl* declVar = new VariableDecl(tokenInRow[i-1].category, tokenInRow[i].token);
                if(formalList == NULL){
                     formalList = declVar;
                     currentFormal = declVar;
                }else{
                     currentFormal->formal = declVar;
                     currentFormal = currentFormal->formal;
                }
            }else{
                printf("parse formal error, expect ident\n");
                exit(0);
            }
        }else if(formal_type_start == 2){
            if(tokenInRow[i].category==T_Comma){
                formal_type_start = 0;
            }else if(tokenInRow[i].category == T_RPara){
                formal_type_start = 3;
            }else{
                printf("parse formal error, expect , or ) \n");
                exit(0);
            }
        }else if(formal_type_start == 3){
            if(tokenInRow[i].category==T_LCurvePara){
                formal_type_start = 0;
                return formalList;
            }else{
                printf("parse formal error, expect { .\n");
                exit(0);
            }
        }
    }
    return NULL;
};

bodyStmt* FunctionDecl::parseStmtBlock(FILE* input_file){
        bodyStmt* retPointer=NULL;
	while(fgets(szLineBuffer, MAX_LINE_SIZE, input_file)!=NULL){
		row_index++;
		if(szLineBuffer[0] == '\n') continue;
		rowTokenNum = getTokens(szLineBuffer, row_index);//tokens for this row in array.
                if(tokenInRow[0].category==T_Int || tokenInRow[0].category==T_Double ||
                   tokenInRow[0].category==T_Bool|| tokenInRow[0].category==T_String){
                        if(tokenInRow[1].category == T_Identifier){
                             if(tokenInRow[2].category == T_SemiColon){
                                 VariableDecl* declVar = new VariableDecl(tokenInRow[0].category, tokenInRow[1].token);
                                 bodyStmt*     declTmp = new bodyStmt(STMT_VAR,(void*)declVar);
                                 if(retPointer == NULL){
                                      retPointer = declTmp;
                                      stmtFirstInfor = declTmp;
                                      stmtCurrentInfor = declTmp;
                                 }else{
                                      stmtCurrentInfor->next = declTmp;
                                      stmtCurrentInfor=stmtCurrentInfor->next;
                                 }
                              }else{
                                   printf("Declaration in stmt wrong, expect ; \n");
                                   return false;
                              }
                         }else{
                              printf("Declaration in stmt wrong, expect identifier\n");
                              return false;
                         }
                }else if(tokenInRow[0].category==T_Identifier     ||
                         tokenInRow[0].category==T_Logic_Not      ||
                         tokenInRow[0].category==T_Bitwise_Not    ||
                         tokenInRow[0].category==T_Sub            ||
                         tokenInRow[0].category==T_LPara          ||
                         tokenInRow[0].category==T_NULL           ||
                         tokenInRow[0].category==T_BoolConstant   ||
                         tokenInRow[0].category==T_IntConstant    ||
                         tokenInRow[0].category==T_DoubleConstant ||
                         tokenInRow[0].category==T_StringConstant ||
                         tokenInRow[0].category==T_ReadInteger    ||
                         tokenInRow[0].category==T_ReadLine       ){
                         /*if(tokenInRow[1].category==T_Assign){//assignment
                             new assignment();
                             assignment->right=parseExpression();
                         }else if(tokenInRow[1].category==T_LPara){//func call
                             new call();
                             call->actuals=parseActuals();
                         }else if(tokenInRow[1].category==T_SemiColon){//ident

                         }*/
                         Expr*        expr = new Expr();
                         bodyStmt* exprTmp = new bodyStmt(STMT_EXPR,(void*)expr);
                         expr->parseExpr();
                         if(retPointer == NULL){
                              retPointer = exprTmp;
                              stmtFirstInfor = exprTmp;
                              stmtCurrentInfor = exprTmp;
                         }else{
                              stmtCurrentInfor->next = exprTmp;
                              stmtCurrentInfor=stmtCurrentInfor->next;
                         }
                }else if(tokenInRow[0].category==T_If){
                }else if(tokenInRow[0].category==T_While){
                }else if(tokenInRow[0].category==T_For){
                }else if(tokenInRow[0].category==T_Return){
                }else if(tokenInRow[0].category==T_Break){
                }else if(tokenInRow[0].category==T_Print){
                }else if(tokenInRow[0].category==T_LCurvePara){//StmtBlock
                }else if(tokenInRow[0].category==T_RCurvePara){//StmtBlockEnd
                }else if(tokenInRow[0].category==T_Identifier){//Expr

                }
        }
        return retPointer;
}

//bodyStmt* FunctionDecl::parseStmtBlock(FILE* input, int index){

//}

class Decl{
      public:
          Decl(int type){
               declType = type;
               ident[0] = '\0';
               category = 0;
               declPointer = NULL;
               nextItem    = NULL;  
          };
          Decl(int type, char* name, void* decl, int categ){
               declType = type;
               strcpy(ident, name);
               declPointer = decl;
               category = categ;
          };
          Decl(int cat, int type){
               category = cat;
               if(category == DECL_VAR){
                    declPointer = (void*)new VariableDecl(type);
                    declType = type;     
               }else if(category == DECL_FUNC){
                    declPointer = (void*)new FunctionDecl(type);
                    declType = type;
               }else{
                    declPointer = NULL;
               }
          };
          ~Decl(){
               if(category == DECL_VAR)  {delete (VariableDecl*)declPointer; category=0;}
               if(category == DECL_FUNC) {delete (FunctionDecl*)declPointer; category=0;}
               if(nextItem != NULL)      {delete (nextItem); nextItem=NULL;}
          }
      public:
          int declType;//int, double, bool, string
          char ident[MAX_TOKEN_SIZE];//1000
          void* declPointer;//VariableDecl, FunctionDecl
          int category; //1: var, 2: func
          Decl* nextItem;
};

/* Program is the root node of AST, and it is actually a linked list of declarations.
 * Both variable declaration and Function declaration.
 *
 * declFirst: specify the start declaration in the linked list.
 * declTotal: is the number of declarations in this linked list.
*/
class Program{
      public: 
      Program(){
           declTableStart = NULL;
           declTotal      = 0;
           curStatus      = STATUS_PROGRAM;
      };
      ~Program(){
           if(declTableStart != NULL){
               delete declTableStart;
               declTableStart = NULL;
           }
      };
      bool parseProgram(FILE* input_file);
      void printAST();
      public:
           Decl  *declTableStart;
           Decl  *declTableCurrent;
           int    declTotal;
           int    curStatus;
};

bool Program::parseProgram(FILE* input_file){
	row_index = 0;
	while(fgets(szLineBuffer, MAX_LINE_SIZE, input_file)!=NULL){
		row_index++;
		if(szLineBuffer[0] == '\n') continue;
		rowTokenNum = getTokens(szLineBuffer, row_index);//tokens for this row in array.
                if(rowTokenNum==3){//parse Variable start
                     if(tokenInRow[0].category==T_Int || tokenInRow[0].category==T_Double ||
                        tokenInRow[0].category==T_Bool|| tokenInRow[0].category==T_String){
                        if(tokenInRow[1].category == T_Identifier){
                             if(tokenInRow[2].category == T_SemiColon){
                                 VariableDecl* declVar = new VariableDecl(tokenInRow[0].category, tokenInRow[1].token);
                                 Decl*         declTmp = new Decl(tokenInRow[0].category, tokenInRow[1].token, declVar, DECL_VAR);
                                 if(declTableStart == NULL){
                                      declTableStart = declTmp;
                                      declTableCurrent = declTmp;
                                 }else{
                                      declTableCurrent->nextItem = declTmp;
                                      declTableCurrent = declTableCurrent->nextItem;
                                 }
                             }else{
                                  printf("Declaration errror, expect a semiColon\n");
                                  return false;
                             }
                        }else{
                             printf("Declaration errror, expect an identifier\n");
                             return false;
                        }
                     }else{
                          printf("Declaration Error. expect variable type\n");
                          return false;
                     }
                }else if(rowTokenNum>=5){//parse variable end
                     if(tokenInRow[0].category==T_Int || tokenInRow[0].category==T_Double ||
                        tokenInRow[0].category==T_Bool|| tokenInRow[0].category==T_String ||
                        tokenInRow[0].category==T_Void){
                        if(tokenInRow[1].category == T_Identifier){
                             if(tokenInRow[2].category == T_LPara){
                                 FunctionDecl* declFunc = new FunctionDecl(tokenInRow[0].category, tokenInRow[1].token);
                                 Decl*         declTmp  = new Decl(tokenInRow[0].category, tokenInRow[1].token, declFunc, DECL_FUNC);
                                 if(declTableStart == NULL){
                                      declTableStart = declTmp;
                                      declTableCurrent = declTmp;
                                 }else{
                                      declTableCurrent->nextItem = declTmp;
                                      declTableCurrent = declTableCurrent->nextItem;
                                 }
                                 if(tokenInRow[3].category == T_RPara ){
                                       if(tokenInRow[4].category == T_LCurvePara){//no formal
                                            declFunc->stmtFirstInfor = declFunc->parseStmtBlock(input_file);//the 4rd ele was not deal with.
                                       }else{
                                            printf("Declaration Error. expect {\n");
                                            return false;
                                       }
                                 }else{
                                      declFunc->formal = declFunc->parseFormals(input_file, 3);//start from the index 3.
                                      declFunc->stmtFirstInfor = declFunc->parseStmtBlock(input_file);
                                 }
                             }else{
                                 printf("Declaration Error. expect (\n");
                                 return false;
                             }
                        }else{
                             printf("Declaration errror, expect an identifier\n");
                             return false;
                        }
                     }else{
                         printf("Declaration errror, expect an identifier\n");
                         return false;
                     }
                }else{
                     printf("Declaration errror, type and name should be in same row for declaration\n");
                     return false;
                }
	}
        return true;
};

void Program::printAST(){
           Decl* curDeclP = declTableStart;
           while(curDeclP != NULL){
                    if(curDeclP->category == DECL_VAR){
                         printf("type: %d\tident:%s\n",  curDeclP->declType, curDeclP->ident);
                    }else if(curDeclP->category == DECL_FUNC){
                         FunctionDecl* function = (FunctionDecl*)(curDeclP->declPointer);
                         printf("functype: %d\tname:%s\n", function->type, function->ident);
                         VariableDecl *formal = function->formal;
                         while(formal != NULL){
                              printf("\tFormals:%d\t %s\t", formal->type, formal->ident);
                              formal = formal->formal;
                         }
                         printf("\n");
                         bodyStmt *stmtInfor = function->stmtFirstInfor;
                         while(stmtInfor != NULL){
                              if(stmtInfor->category == STMT_VAR){
                                  printf("\tFunc body:%d\t %s\t", ((VariableDecl*)(stmtInfor->stmtPointer))->type, ((VariableDecl*)(stmtInfor->stmtPointer))->ident);
                              }
                              stmtInfor = stmtInfor->next;
                         }
                         printf("\n");
                    }else{
                         printf("Declaration Category Error 0.\n");
                    }
               curDeclP = curDeclP->nextItem;
           }

};
