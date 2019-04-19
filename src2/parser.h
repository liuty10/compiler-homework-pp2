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
             int parseExpr(int startPos);
       public:
             int    selfcategory;
             Expr*  left;
             int    leftCategory;
             char   op[2];//two bye to save operators
             Expr*  right;
             int    rightCategory;
             Expr*  next;  // The last two rows are for call(actuals) only,
             int    nextCategory;// since actuals can be a list of Exprs.
};
//TODO:
int Expr::parseExpr(int startPos){
    //return type of Expr
    /*if(tokenInRow[1].category==T_Assign){//assignment
        new assignment();
        assignment->right=parseExpression();
    }else if(tokenInRow[1].category==T_LPara){//func call
        new call();
        call->actuals=parseActuals();
    }else if(tokenInRow[1].category==T_SemiColon){//ident

    }*/

};

// STMT_BLOCK
// we need a linked list to save each stmt infor
// This is actually an infor table for all statements.
class bodyStmt{
       public:
            bodyStmt(){
                   category = 0;
                   stmtPointer = NULL;
            };
            bodyStmt(int categ, void* pointer){
                   category = categ;
                   stmtPointer = pointer;
            };
            ~bodyStmt(){

            };
       public:
           int category;
           void* stmtPointer;//varDecl,if,while,for,break,return,print,expr,
           bodyStmt* next;
};

// root class of all statements.
class Stmt{
      public:
           bodyStmt* parseStmtBlock(FILE* input_file);
      public:
           int multipleStmtFlag;
           bodyStmt* stmtCurrentInfor;
      private:
           void findSemiColonPos(int *firstPos, int *secondPos){
                int i=0;
                *firstPos  = 0;
                *secondPos = 0;
                for(i=0;i<rowTokenNum;i++){
                    if(tokenInRow[i].category == T_SemiColon && (*firstPos)==0)
                        *firstPos = i;
                    else if(tokenInRow[i].category == T_SemiColon && (*secondPos)==0)
                        *secondPos = i;
                    else
                        ;
                }
           }
};

// STMT_IF
class ifStmt:public Stmt{
       public:
             ifStmt(Expr* condition, int categ, bodyStmt* ifstart, bodyStmt* elsestmt){
                  cond     = condition;
                  category = categ;
                  ifstmt   = ifstart;
                  elsestmt = elsestmt;
             };
             ~ifStmt(){

             }
       public:
             int category;
             Expr* cond;
             bodyStmt* ifstmt;
             bodyStmt* elsestmt;
};

// STMT_WHILE
class whileStmt:public Stmt{
//class whileStmt::public Stmt{
       public:
             whileStmt(Expr* condition, int categ, bodyStmt* whilestmt){
                  cond         = condition;
                  condCategory = categ;
                  stmt         = whilestmt;
             }
             ~whileStmt(){

             }
       public:
       int condCategory;
       Expr* cond;
       bodyStmt* stmt;
};

// STMT_FOR
class forStmt:public Stmt{
      public:
            forStmt(Expr* initExpr, Expr* condExpr, Expr* updateExpr,
                    int initCateg, int condCateg, int updateCateg, bodyStmt* forstmt){

                            init = initExpr;
                            cond = condExpr;
                          update = updateExpr;
                  initCategory   = initCateg;
                  condCategory   = condCateg;
                  updateCategory = updateCateg;
            };
            ~forStmt(){

            }
      public:
            int initCategory;
            int condCategory;
            int updateCategory;
            Expr* init;
            Expr* cond;
            Expr* update;
            bodyStmt* stmt;
};

// STMT_RET
class retStmt{
      public:
            retStmt(Expr* ret, int categ){
                 category = categ;
                 retExpr  = ret;
            };
            ~retStmt(){

            };
      public:
            int category;
            Expr* retExpr;
};

// STMT_BREAK
struct breakStmt{
      struct Expr* beak; 
};

// STMT_PRINT
class printStmt{
       public:
            printStmt(){
            };
            void parseActuals();
            int getArgcNum(){
                int i=0;
                int argc_num=0;
                for(i=0;i<rowTokenNum;i++){
                   if(tokenInRow[i].category == T_Comma){
                      argc_num++;
                   }
                }
                return argc_num;
            };
       public:
            Expr* actualList;
            Expr* cur_actual;
};

void printStmt::parseActuals(){
     int i = 0;
     int argc_num = getArgcNum();
     for(i=0;i<argc_num;i++){
        Expr* expr = new Expr();
        expr->parseExpr(2);
        if(actualList == NULL){
             actualList = expr;
             cur_actual = expr;
        }else{
             cur_actual->next = expr;
             cur_actual = cur_actual->next;
        }
     }
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

class FunctionDecl:public Stmt{
       public:
           FunctionDecl(int declType){
               type = declType;
               ident[0] = '\0';
               formal = NULL;
               funcstmt   = NULL;
               //stmtFirstInfor   = NULL;
           };
           FunctionDecl(int declType, char* name){
               type = declType;
               strcpy(ident, name);
               formal = NULL;
               funcstmt   = NULL;
               //stmtFirstInfor   = NULL;
           };
           ~FunctionDecl(){
                if(formal != NULL) {delete formal; formal=NULL;}
                if(funcstmt   != NULL) {delete funcstmt; funcstmt=NULL;}
                //if(stmtFirstInfor   != NULL) {delete stmtFirstInfor; stmtFirstInfor=NULL;}
           };
       VariableDecl* parseFormals(FILE* input_file, int index);
       //static bodyStmt* parseStmtBlock(FILE* input);
       //bodyStmt* parseStmtBlock(FILE* input, int index);
       public:
           int type;//int, double, bool, string, void
           char ident[MAX_TOKEN_SIZE];//1000
           VariableDecl* formal;//a list of varaibles
           bodyStmt* funcstmt;
           //bodyStmt* stmtFirstInfor;
           //bodyStmt* stmtCurrentInfor;
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

bodyStmt* Stmt::parseStmtBlock(FILE* input_file){
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
                         Expr*        expr = new Expr();
                         bodyStmt* exprTmp = new bodyStmt(STMT_EXPR,(void*)expr);
                         expr->parseExpr(0);
                         if(retPointer == NULL){
                              retPointer = exprTmp;
                              stmtCurrentInfor = exprTmp;
                         }else{
                              stmtCurrentInfor->next = exprTmp;
                              stmtCurrentInfor=stmtCurrentInfor->next;
                         }
                }else if(tokenInRow[0].category==T_If){
                      Expr*          cond = new Expr();
                      bodyStmt*   ifStart = new bodyStmt();
                      bodyStmt* elseStart = new bodyStmt();
                      ifStmt*      ifstmt = new ifStmt(cond, cond->parseExpr(2),ifStart, elseStart);
                      bodyStmt*    IfTmp  = new bodyStmt(STMT_IF,(void*)ifstmt);
                      //if there is a "{" in if stmt, multipleStmtFlag = 1;
                      //else multipleStmtFlag = 0;
                      //before parsing next line, complete the remaining tokens in array first.
                      ifStart   = ifstmt->parseStmtBlock(input_file);
                      if(rowTokenNum)
                      if(tokenInRow[1].category == T_Else){
                               elseStart = ifstmt->parseStmtBlock(input_file);
                      }
                      if(retPointer == NULL){
                           retPointer = IfTmp;
                           stmtCurrentInfor = IfTmp;
                      }else{
                           stmtCurrentInfor->next = IfTmp;
                           stmtCurrentInfor=stmtCurrentInfor->next;
                      }
                }else if(tokenInRow[0].category==T_While){
                      Expr*            cond = new Expr();
                      bodyStmt*  whileStart = new bodyStmt();
                      whileStmt*  whilestmt = new whileStmt(cond, cond->parseExpr(2), whileStart);
                      bodyStmt*    WhileTmp = new bodyStmt(STMT_WHILE,(void*)whilestmt);
                      whileStart = whilestmt->parseStmtBlock(input_file);

                      if(retPointer == NULL){
                           retPointer = WhileTmp;
                           stmtCurrentInfor = WhileTmp;
                      }else{
                           stmtCurrentInfor->next = WhileTmp;
                           stmtCurrentInfor = stmtCurrentInfor->next;
                      }
                }else if(tokenInRow[0].category==T_For){
                      Expr*          init = new Expr();
                      Expr*          cond = new Expr();
                      Expr*        update = new Expr();
                      bodyStmt*  forStart = new bodyStmt();
                      int firstSemi, secondSemi;
                      firstSemi=secondSemi=0;
                      findSemiColonPos(&firstSemi, &secondSemi);
                      forStmt*    forstmt = new forStmt(init, cond, update,
                                                        init->parseExpr(2), cond->parseExpr(firstSemi+1),
                                                        update->parseExpr(secondSemi+1),forStart);

                      bodyStmt*    ForTmp = new bodyStmt(STMT_FOR,(void*)forstmt);
                      forStart=forstmt->parseStmtBlock(input_file);
                      if(retPointer == NULL){
                           retPointer = ForTmp;
                           stmtCurrentInfor = ForTmp;
                      }else{
                           stmtCurrentInfor->next = ForTmp;
                           stmtCurrentInfor = stmtCurrentInfor->next;
                      }
                }else if(tokenInRow[0].category==T_Return){
                      Expr*     retexpr = new Expr();
                      retStmt*  retstmt = new retStmt(retexpr, retexpr->parseExpr(1));
                      bodyStmt*  RetTmp = new bodyStmt(STMT_RET,(void*)retstmt);

                      if(retPointer == NULL){
                           retPointer = RetTmp;
                           stmtCurrentInfor = RetTmp;
                      }else{
                           stmtCurrentInfor->next = RetTmp;
                           stmtCurrentInfor = stmtCurrentInfor->next;
                      }
                }else if(tokenInRow[0].category==T_Break){
                     if(tokenInRow[1].category==T_SemiColon){
                         bodyStmt*  BreakTmp = new bodyStmt(STMT_BREAK, (void*)NULL);
                         if(retPointer == NULL){
                              retPointer = BreakTmp;
                              stmtCurrentInfor = BreakTmp;
                         }else{
                              stmtCurrentInfor->next = BreakTmp;
                              stmtCurrentInfor = stmtCurrentInfor->next;
                         }
                         
                     }else{
                         printf("Parsing Error, expect a ; \n");
                         exit(0);
                     }
                }else if(tokenInRow[0].category==T_Print){
                      if(tokenInRow[1].category==T_LPara){
                          printStmt* printstmt = new printStmt();
                          bodyStmt*   PrintTmp = new bodyStmt(STMT_PRINT,(void*)printstmt);
                          printstmt->parseActuals();

                          if(retPointer == NULL){
                               retPointer = PrintTmp;
                               stmtCurrentInfor = PrintTmp;
                          }else{
                               stmtCurrentInfor->next = PrintTmp;
                               stmtCurrentInfor = stmtCurrentInfor->next;
                          }
                      }else{
                           printf("Parsing Error, expect a ( \n");
                           exit(0);
                      }
                }else if(tokenInRow[0].category==T_LCurvePara){//StmtBlock
                      bodyStmt*  blockstmt = new bodyStmt(STMT_BLOCK,(void*)NULL);
                      bodyStmt*  BlockTmp  = new bodyStmt(STMT_BLOCK,(void*)blockstmt);
                      //declFunc->funcstmt = declFunc->parseStmtBlock(input_file);//the 4rd ele was not deal with.

                      /*if(retPointer == NULL){
                           retPointer = RetTmp;
                           stmtCurrentInfor = RetTmp;
                      }else{
                           stmtCurrentInfor->next = RetTmp;
                           stmtCurrentInfor = stmtCurrentInfor->next;
                      }*/
                      
                }else if(tokenInRow[0].category==T_RCurvePara){//StmtBlockEnd
                      return retPointer;
                }else {//Expr
                      printf("Unexpected case, token category: %d\n", tokenInRow[0].category);
                      exit(0);
                }
        }
        return retPointer;
};

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
                                            declFunc->funcstmt = declFunc->parseStmtBlock(input_file);//the 4rd ele was not deal with.
                                            //declFunc->stmtFirstInfor = declFunc->parseStmtBlock(input_file);//the 4rd ele was not deal with.
                                       }else{
                                            printf("Declaration Error. expect {\n");
                                            return false;
                                       }
                                 }else{
                                      declFunc->formal = declFunc->parseFormals(input_file, 3);//start from the index 3.
                                      declFunc->funcstmt = declFunc->parseStmtBlock(input_file);
                                      //declFunc->stmtFirstInfor = declFunc->parseStmtBlock(input_file);
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
                         bodyStmt *stmtInfor = function->funcstmt;
                         //bodyStmt *stmtInfor = function->stmtFirstInfor;
                         while(stmtInfor != NULL){
                              if(stmtInfor->category == STMT_VAR){
                                  printf("\tFunc body:%d\t %s\t", ((VariableDecl*)(stmtInfor->stmtPointer))->type, ((VariableDecl*)(stmtInfor->stmtPointer))->ident);
                              }else if(stmtInfor->category == STMT_BREAK){
                                  printf("\tFunc body:%s\n", "break");
                              }else{

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
