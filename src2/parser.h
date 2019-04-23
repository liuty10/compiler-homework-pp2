/* File: parser.c 
 * --------------
 * This file implements the parser for the grammar.
 */

#include "define.h"

extern struct token tokenInRow[MAX_TOKEN_IN_ROW];
extern int getTokens(char *inputLine, int cur_row);
bool parser(FILE* input, FILE* output);
char szLineBuffer[MAX_LINE_SIZE + 1];//input buffer for fgets
int rowTokenNum;
int row_index;
int nextRowError = 0;
int globalNextParsePos = -1;
//EXPR_CONSTANT
//EXPR_IDENT
class constIdentOperatorNode{
       public:
           constIdentOperatorNode(int categ, char* content){
                category = categ;
                strcpy(ident, content);
           };
           ~constIdentOperatorNode(){

           };
       public:
           int  category;
           char ident[MAX_TOKEN_SIZE];
           constIdentOperatorNode* left;
           constIdentOperatorNode* right;
};
//STMT_EXPR
class Expr{
       public:
             Expr(){

             };
             ~Expr(){};
             constIdentOperatorNode* parseExpr(int startPos, int endPoint);
             int leftLowestOperator(int* op, int startPos, int endPos);
       public:
             int    selfcategory;
             constIdentOperatorNode* exprHeadNode;
             int    nextCategory;// since actuals can be a list of Exprs.
             Expr*  next;  // The last two rows are for call(actuals) only,
};

int Expr::leftLowestOperator(int* op, int startPos, int endPos){
    int i;
    int lowestPos= -1;
    int lowestOp = 36;
    for(i=startPos; i<=endPos; i++){
        if(tokenInRow[i].category >= 21 && tokenInRow[i].category <=35){
             if(tokenInRow[i].category < lowestOp){
                  lowestOp  = tokenInRow[i].category;
                  lowestPos = i;
             }else{
                  continue;
             }
        }else{
             continue;
        }
    }
    if(lowestOp == 36){
        return -1;
    }else{
        *op = lowestOp;
        return lowestPos;
    }
};

// STMT_PRINT
class printStmt{
       public:
            printStmt(){
            };
            void parseActuals(int start, int end);
            int getArgcNum(int commaPos[], int start, int end){
                int i=0;
                int argc_num=0;
                commaPos[0] = 2;
                for(i=start;i<end;i++){
                   if(tokenInRow[i].category == T_Comma){
                      argc_num++;
                      commaPos[argc_num] = i+1;
                   }
                }
                if(start == end){
                      commaPos[0]=start;
                }
                commaPos[argc_num+1] = end;
                return (argc_num+1);
            };
       public:
            Expr* actualList;
            Expr* cur_actual;
};


// STMT_funcall
class funcCall:public printStmt{
       public:
            funcCall(char* name){
                strcpy(ident, name);
            };
       public:
            char ident[MAX_TOKEN_SIZE];
};


constIdentOperatorNode* Expr::parseExpr(int startPos, int endPos){
    int op = -1;
    int pos= -1;
    if(tokenInRow[startPos].category == T_LPara && tokenInRow[endPos].category == T_RPara){
         startPos++;
         endPos--;
    }
    pos = leftLowestOperator(&op, startPos, endPos);
    if(pos == -1){//not find
          if(tokenInRow[startPos].category == T_Identifier &&
             tokenInRow[startPos+1].category == T_LPara    ){//this is a function
               constIdentOperatorNode* exprHeadNode_tmp = new constIdentOperatorNode(STMT_CALL,tokenInRow[startPos].token);
               funcCall* funcstmt = new funcCall(tokenInRow[startPos].token);
               funcstmt->parseActuals(startPos+2,endPos-1);
               exprHeadNode_tmp->left = (constIdentOperatorNode*)funcstmt;
               globalNextParsePos = startPos;
               return exprHeadNode_tmp;
          }
          globalNextParsePos = startPos;
          return new constIdentOperatorNode(tokenInRow[startPos].category, tokenInRow[startPos].token);
    }else if(pos == startPos){
          if(tokenInRow[startPos].category==T_Sub || tokenInRow[startPos].category == T_Logic_Not){
               constIdentOperatorNode* exprHeadNode_tmp = new constIdentOperatorNode(op,tokenInRow[pos].token);
               exprHeadNode_tmp->left = NULL;//left hand side
               exprHeadNode_tmp->right= parseExpr(pos+1, endPos);//right hand side
               return exprHeadNode_tmp;
          }else{
               printf("parsing expr error, it's not a unary\n");
          }
    }else{//find 
          constIdentOperatorNode* exprHeadNode_tmp = new constIdentOperatorNode(op,tokenInRow[pos].token);
          exprHeadNode_tmp->left = parseExpr(startPos, pos-1);//left hand side
          exprHeadNode_tmp->right= parseExpr(pos+1, endPos);//right hand side
          return exprHeadNode_tmp;
    }
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
             ifStmt(Expr* condition, bodyStmt* ifstart, bodyStmt* elsestmt){
                  cond     = condition;
                  ifstmt   = ifstart;
                  elsestmt = elsestmt;
             };
             ~ifStmt(){

             }
       public:
             //int category;
             Expr* cond;
             bodyStmt* ifstmt;
             bodyStmt* elsestmt;
};

// STMT_WHILE
class whileStmt:public Stmt{
//class whileStmt::public Stmt{
       public:
             whileStmt(Expr* condition, bodyStmt* whilestmt){
                  cond         = condition;
                  //condCategory = categ;
                  stmt         = whilestmt;
             }
             ~whileStmt(){

             }
       public:
       //int condCategory;
       Expr* cond;
       bodyStmt* stmt;
};

// STMT_FOR
class forStmt:public Stmt{
      public:
            forStmt(Expr* initExpr, Expr* condExpr, Expr* updateExpr, bodyStmt* forstmt){

                            init = initExpr;
                            cond = condExpr;
                          update = updateExpr;
            };
            ~forStmt(){

            }
      public:
            Expr* init;
            Expr* cond;
            Expr* update;
            bodyStmt* stmt;
};

// STMT_RET
class retStmt{
      public:
            //retStmt(Expr* ret, int categ){
            retStmt(Expr* ret){
                 //category = categ;
                 retExpr  = ret;
            };
            ~retStmt(){

            };
      public:
            //int category;
            Expr* retExpr;
};

// STMT_BREAK
struct breakStmt{
      struct Expr* beak; 
};

void printStmt::parseActuals(int start, int end){
     int i = 0;
     int commaPos[100];
     if(tokenInRow[2].category == T_RPara){
         if(tokenInRow[3].category == T_SemiColon){
              return;//no arguments
         }else{
              printf("Parse print error, expect ;\n");
              exit(0);
         }
     }
     int argc_num = getArgcNum(commaPos, start, end);
     for(i=0;i<argc_num;i++){
        Expr* expr = new Expr();
        if(tokenInRow[i].category == T_String){
             expr->selfcategory = T_String;
             expr->exprHeadNode = new constIdentOperatorNode(T_String, tokenInRow[i].token);
        }else{
             expr->exprHeadNode = expr->parseExpr(commaPos[i],commaPos[i+1]-2);
        }
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
       public:
           int type;//int, double, bool, string, void
           char ident[MAX_TOKEN_SIZE];//1000
           VariableDecl* formal;//a list of varaibles
           bodyStmt* funcstmt;
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
                printf("\n*** Error line %d.\n", tokenInRow[i].row);
                int j;
                printf("%s ",szLineBuffer);
                for(j=0;j<tokenInRow[i-1].right-1;j++){
                    printf(" ");
                }
                printf("^\n");
                printf("*** syntax error\n\n");
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
                if(nextRowError == 1){
                      printf("\n*** Error line %d.\n", tokenInRow[0].row);
                      int j;
                      printf("%s ",szLineBuffer);
                      for(j=0;j<tokenInRow[0].left-2;j++){
                          printf(" ");
                      }
                      printf("^\n");
                      printf("*** syntax error\n\n");
                      exit(0);
                }
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
                }else if(tokenInRow[0].category==T_Identifier && tokenInRow[1].category==T_LPara){
                      funcCall*     funcall = new funcCall(tokenInRow[0].token);
                      funcall->parseActuals(0, rowTokenNum-1);
                      bodyStmt*     CallTmp = new bodyStmt(STMT_CALL,(void*)funcall);
                      if(tokenInRow[rowTokenNum-1].category != T_SemiColon){
                          nextRowError = 1;
                          continue;
                      }
                      if(retPointer == NULL){
                           retPointer = CallTmp;
                           stmtCurrentInfor = CallTmp;
                      }else{
                           stmtCurrentInfor->next = CallTmp;
                           stmtCurrentInfor = stmtCurrentInfor->next;
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
                         if(tokenInRow[rowTokenNum-1].category != T_SemiColon){
                             nextRowError = 1;
                             continue;
                         }
                         Expr*        expr = new Expr();
                         bodyStmt* exprTmp = new bodyStmt(STMT_EXPR,(void*)expr);
                         expr->exprHeadNode=expr->parseExpr(0,rowTokenNum-2);
                         if(globalNextParsePos != rowTokenNum-2 && tokenInRow[rowTokenNum-2].category != T_RPara){
                             printf("\n*** Error line %d.\n", tokenInRow[globalNextParsePos].row);
                             int j;
                             printf("%s ",szLineBuffer);
                             for(j=0;j<tokenInRow[globalNextParsePos].right-1;j++){
                                 printf(" ");
                             }
                             printf("^\n");
                             printf("*** syntax error\n\n");
                             exit(0);
                         }
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
                      cond->exprHeadNode  = cond->parseExpr(2, rowTokenNum-1);
                      ifStmt*      ifstmt = new ifStmt(cond, ifStart, elseStart);
                      bodyStmt*    IfTmp  = new bodyStmt(STMT_IF,(void*)ifstmt);
                      
                      //if there is a "{" in if stmt, multipleStmtFlag = 1;
                      //else multipleStmtFlag = 0;
                      //before parsing next line, complete the remaining tokens in array first.
                      if(tokenInRow[globalNextParsePos+1].category != T_RPara){
                           printf("Error, expect ) for if. \n");
                           exit(0);
                      }
                      /*
                      if(globalNextParsePos+2 <= rowTokenNum-1){
                         if(tokenInRow[globalNextParsePos+2].category != T_LCurvePara){
                              multiplaStmtFlag = 0;
                         }else{
                              multiplaStmtFlag = 1;
                              ifStart   = ifstmt->parseStmtBlock(input_file);
                         }
                      }else{
                              multiplaStmtFlag = 0;
                              ifStart   = ifstmt->parseStmtBlock(input_file);
                      }

                      for(i=globalNextParsePos+1;i<rowTokenNum-1;i++){
                            if(tokenInRow[i].category == T_RPara){
                                if(i+1<=rowTokenNum-1){//we check LCurvePara
                                    if(tokenInRow[i+1].category == T_RCurvePara){
                                         
                                    }else{
                                         
                                    }
                                }else{//only one stmt
                                    multiplaStmtFlag = 0;
                                }
                            }else{
                                printf("Error, expect ) for if. \n");
                            }
                      }
                      if(tokenInRow[globalNextParsePos+1].category == T_LCurvePara){
                            multipleStmtFlag = 1;
                            continue;
                      }
                      if(tokenInRow[rowTokenNum-1].category == T_RPara){
                            multipleStmtFlag = 0;
                            
                      }
                      if(tokenInRow[rowTokenNum-1].category == T_SemiColon){
                      }
                      if(tokenInRow[rowTokenNum-1].category == T_LCurvePara){
                      }
                      if(tokenInRow[rowTokenNum-1].category == T_RCurvePara){
                      }*/
                      ifStart   = ifstmt->parseStmtBlock(input_file);
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
                      cond->exprHeadNode    = cond->parseExpr(2, rowTokenNum-1);
                      whileStmt*  whilestmt = new whileStmt(cond, whileStart);
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
                      init->exprHeadNode   = init->parseExpr(2, firstSemi);
                      cond->exprHeadNode   = cond->parseExpr(firstSemi+1, secondSemi);
                      update->exprHeadNode = update->parseExpr(secondSemi+1, rowTokenNum-1);
                      forStmt*    forstmt  = new forStmt(init, cond, update, forStart);
                      bodyStmt*    ForTmp  = new bodyStmt(STMT_FOR,(void*)forstmt);
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
                      retexpr->exprHeadNode = retexpr->parseExpr(1, rowTokenNum-2);
                      retStmt*  retstmt = new retStmt(retexpr);
                      bodyStmt*  RetTmp = new bodyStmt(STMT_RET,(void*)retstmt);
                      if(tokenInRow[rowTokenNum-1].category != T_SemiColon){
                          nextRowError = 1;
                          continue;
                      }

                      if(retPointer == NULL){
                           retPointer = RetTmp;
                           stmtCurrentInfor = RetTmp;
                      }else{
                           stmtCurrentInfor->next = RetTmp;
                           stmtCurrentInfor = stmtCurrentInfor->next;
                      }
                }else if(tokenInRow[0].category==T_Break){
                      if(tokenInRow[rowTokenNum-1].category != T_SemiColon){
                          nextRowError = 1;
                          continue;
                      }
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
                          if(tokenInRow[rowTokenNum-1].category != T_SemiColon){
                            nextRowError = 1;
                            continue;
                          }
                          printStmt* printstmt = new printStmt();
                          bodyStmt*   PrintTmp = new bodyStmt(STMT_PRINT,(void*)printstmt);
                          printstmt->parseActuals(0, rowTokenNum-1);

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
                      printf("\n*** Error line %d.\n", tokenInRow[0].row);
                      int j;
                      printf("%s ",szLineBuffer);
                      for(j=0;j<tokenInRow[0].left-2;j++){
                          printf(" ");
                      }
                      printf("^\n");
                      printf("*** syntax error\n\n");
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
      void printAnExpr(constIdentOperatorNode* node, int space);
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
                                       }else{
                                            printf("Declaration Error. expect {\n");
                                            return false;
                                       }
                                 }else{
                                      declFunc->formal = declFunc->parseFormals(input_file, 3);//start from the index 3.
                                      declFunc->funcstmt = declFunc->parseStmtBlock(input_file);
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

void Program::printAnExpr(constIdentOperatorNode* node, int space){
     int i;
     switch(node->category){
           case T_Assign:
                for(i=0;i<space;i++) printf(" ");
                printf("AssignExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: =\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_NULL:
                for(i=0;i<space;i++) printf(" ");
                printf("nullConstant: %s\n", node->ident);
                break;
           case T_BoolConstant:
                for(i=0;i<space;i++) printf(" ");
                printf("BoolConstant: %s\n", node->ident);
                break;
           case T_IntConstant:
                for(i=0;i<space;i++) printf(" ");
                printf("IntConstant: %s\n", node->ident);
                break;
           case T_DoubleConstant:
                for(i=0;i<space;i++) printf(" ");
                printf("DoubleConstant: %s\n", node->ident);
                break;
           case T_StringConstant:
                for(i=0;i<space;i++) printf(" ");
                printf("StringConstant: %s\n", node->ident);
                break;
           case T_Identifier:
                for(i=0;i<space;i++) printf(" ");
                printf("FieldAccess:\n");
                for(i=0;i<space;i++) printf(" ");
                printf("    Identifier: %s\n", node->ident);
                break;
           case T_Logic_Or:
                for(i=0;i<space;i++) printf(" ");
                printf("LogicalExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: ||\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Logic_And:
                for(i=0;i<space;i++) printf(" ");
                printf("LogicalExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: &&\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Logic_Not:
                for(i=0;i<space;i++) printf(" ");
                printf("LogicalExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: !\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Add:
                for(i=0;i<space;i++) printf(" ");
                printf("ArithmeticExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: +\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Sub:
                for(i=0;i<space;i++) printf(" ");
                printf("ArithmeticExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: -\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Mul:
                for(i=0;i<space;i++) printf(" ");
                printf("ArithmeticExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: *\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Div:
                for(i=0;i<space;i++) printf(" ");
                printf("ArithmeticExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: /\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Percent:
                for(i=0;i<space;i++) printf(" ");
                printf("ArithmeticExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: /%\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Equal:
                for(i=0;i<space;i++) printf(" ");
                printf("EqualityExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: ==\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_NotEqual:
                for(i=0;i<space;i++) printf(" ");
                printf("RelationalExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: !=\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Less:
                for(i=0;i<space;i++) printf(" ");
                printf("RelationalExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: <\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_LessEqual:
                for(i=0;i<space;i++) printf(" ");
                printf("RelationalExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: <=\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_Larger:
                for(i=0;i<space;i++) printf(" ");
                printf("RelationalExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: >\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_GreaterEqual:
                for(i=0;i<space;i++) printf(" ");
                printf("RelationalExpr:\n");
                if(node->left != NULL){
                     printAnExpr(node->left, space+4);
                }
                for(i=0;i<space+4;i++) printf(" ");
                printf("Operator: >=\n");
                if(node->right != NULL){
                     printAnExpr(node->right, space+4);
                }
                break;
           case T_ReadInteger:
                for(i=0;i<space;i++) printf(" ");
                printf("ReadIntegerExpr:\n");
                break;
           case T_ReadLine:
                for(i=0;i<space;i++) printf(" ");
                printf("ReadLineExpr:\n");
                break;
           case STMT_CALL:
                //for(i=0;i<space;i++) printf(" ");
                printf("Call:\n");
                for(i=0;i<space;i++) printf(" ");
                printf("    Indentifier:%s\n", node->ident);
                if(node->left != NULL){
                     printAnExpr(((funcCall*)(node->left))->actualList->exprHeadNode, space+4);
                }
           default://function
                break;
     }
};

void Stmt::printABlock(bodyStmt* stmt, int space){
     bodyStmt* cur_stmt = stmt;
     while(cur_stmt != NULL){
          switch(cur_stmt->category){
                case STMT_VAR:
                    printf("            VarDecl:\n");
                    printf("                Type: %d\n", ((VariableDecl*)(stmtInfor->stmtPointer))->type);
                    printf("                Identifier: %s\n", ((VariableDecl*)(stmtInfor->stmtPointer))->ident);
                    break;
                case STMT_BREAK:
                    printf("            BreakStmt:%s\n", "break");
                    break;
                case STMT_RET:
                    printf("            ReturnStmt:\n");
                    printAnExpr(((retStmt*)(stmtInfor->stmtPointer))->retExpr->exprHeadNode, 16);
                    break;
                case STMT_PRINT:
                    printf("            PrintStmt:\n");
                    printStmt* printstmt = ((printStmt*)(stmtInfor->stmtPointer));
                    if(printstmt->actualList!=NULL){
                        Expr* printexpr = printstmt->actualList;
                        while(printexpr){
                            printf("                (args) ");
                            printAnExpr(printexpr->exprHeadNode,16);
                            printexpr = printexpr->next;
                        }
                    }
                case STMT_CALL:
                    printf("            Call:\n");
                    funcCall* callstmt = ((funcCall*)(stmtInfor->stmtPointer));
                    printf("                Identifier: %s\n", callstmt->ident);
                    if(callstmt->actualList!=NULL){
                        Expr* callexpr = callstmt->actualList;
                        while(callexpr){
                            printAnExpr(callexpr->exprHeadNode,16);
                            callexpr = callexpr->next;
                        }
                    }
                case STMT_EXPR:
                    printAnExpr(((Expr*)(stmtInfor->stmtPointer))->exprHeadNode, 12);
                case STMT_WHILE:
                    printf("            WhileStmt:\n");
                    //printf("               (test) RelationalExpr:\n");
                    printAnExpr(((whileStmt*)(stmtInfor->stmtPointer))->cond->exprHeadNode, 16);
                    printf("                 (body) StmtBlock:\n");
                    printABlock(((whileStmt*)(stmtInfor->stmtPointer))->stmt, 16);
                default:
                    break;
          }
          cur_stmt = cur_stmt->next;
     }
};

void Program::printAST(){
           Decl* curDeclP = declTableStart;
           printf("Program:\n");
           while(curDeclP != NULL){
                    if(curDeclP->category == DECL_VAR){
                         printf("    VarDecl:\n");
                         printf("        Type: %d\n        Identifier:%s\n",curDeclP->declType, curDeclP->ident);
                    }else if(curDeclP->category == DECL_FUNC){
                         printf("    FnDecl:\n");
                         FunctionDecl* function = (FunctionDecl*)(curDeclP->declPointer);
                         printf("        (return type) Type:%d\n", function->type);
                         printf("        Identifier: %s\n", function->ident);
                         VariableDecl *formal = function->formal;
                         if(formal!=NULL){
                               printf("        (formals) VarDecl:\n");
                         }
                         while(formal != NULL){
                              printf("            Type: %d\n            Identifier:%s\n",formal->type, formal->ident);
                              formal = formal->formal;
                         }
                         bodyStmt *stmtInfor = function->funcstmt;
                         printf("        (body) StmtBlock:\n");
                         
                         /*while(stmtInfor != NULL){
                              if(stmtInfor->category == STMT_VAR){
                                  printf("            VarDecl:\n");
                                  printf("                Type: %d\n", ((VariableDecl*)(stmtInfor->stmtPointer))->type);
                                  printf("                Identifier: %s\n", ((VariableDecl*)(stmtInfor->stmtPointer))->ident);
                              }else if(stmtInfor->category == STMT_BREAK){
                                  printf("            BreakStmt:%s\n", "break");
                              }else if(stmtInfor->category == STMT_RET){
                                  printf("            ReturnStmt:\n");
                                  printAnExpr(((retStmt*)(stmtInfor->stmtPointer))->retExpr->exprHeadNode, 16);
                              }else if(stmtInfor->category == STMT_PRINT){
                                  printf("            PrintStmt:\n");
                                  printStmt* printstmt = ((printStmt*)(stmtInfor->stmtPointer));
                                  if(printstmt->actualList!=NULL){
                                      Expr* printexpr = printstmt->actualList;
                                      while(printexpr){
                                          printf("                (args) ");
                                          printAnExpr(printexpr->exprHeadNode,16);
                                          printexpr = printexpr->next;
                                      }
                                  }
                              }else if(stmtInfor->category == STMT_CALL){
                                  printf("            Call:\n");
                                  funcCall* callstmt = ((funcCall*)(stmtInfor->stmtPointer));
                                  printf("                Identifier: %s\n", callstmt->ident);
                                  if(callstmt->actualList!=NULL){
                                      Expr* callexpr = callstmt->actualList;
                                      while(callexpr){
                                          printAnExpr(callexpr->exprHeadNode,16);
                                          callexpr = callexpr->next;
                                      }
                                  }
                              }else if(stmtInfor->category == STMT_EXPR){
                                  printAnExpr(((Expr*)(stmtInfor->stmtPointer))->exprHeadNode, 12);
                              }else if(stmtInfor->category == STMT_WHILE){
                                  printf("            WhileStmt:\n");
                                  //printf("               (test) RelationalExpr:\n");
                                  printAnExpr(((whileStmt*)(stmtInfor->stmtPointer))->cond->exprHeadNode, 16);
                                  printf("                 (body) StmtBlock:\n");
                                  printABlock(((whileStmt*)(stmtInfor->stmtPointer))->stmt, 16);
                              }else{
                                   ;
                              }
                              stmtInfor = stmtInfor->next;
                         }*/
                         printf("\n");
                    }else{
                         printf("Declaration Category Error 0.\n");
                    }
               curDeclP = curDeclP->nextItem;
           }

};
