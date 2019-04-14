/* File: parser.c 
 * --------------
 * This file implements the parser for the grammar.
 */

#include "define.h"

/* Here lists definitions of Expr
 *
*/

//STMT_EXPR
struct Expr{
       struct Expr* left;
       int    leftCategory;
       char   op[2];//two bye to save operators
       struct Expr* right;
       int    rightCategory;
       struct Expr* next;  // The last two rows are for call(actuals) only,
       int    nextCategory;// since actuals can be a list of Exprs.
};

//EXPR_ASSIGN
struct assignExpr{
       char ident[MAX_TOKEN_SIZE];
       struct Expr* assignexpr;//right hand side
       int category;
};

//EXPR_CONSTANT
struct constExpr{
       struct Expr* constexpr;
       int category;
};

//EXPR_IDENT
struct identExpr{
       char ident[MAX_TOKEN_SIZE];
};

//EXPR_CALL
struct callExpr{
       char ident[MAX_TOKEN_SIZE];
       struct Expr* actualList; 
};

//EXPR_PARA
struct paraExpr{
       struct Expr* inpara;
       int category;
};

//EXPR_ARITHM
struct arithmExpr{
       struct Expr* left;
       int    leftCategory;
       char   op;           //1 bye to save operator
       struct Expr* right;
       int    rightCategory;
};

//EXPR_EQUAL
struct equalExpr{
       struct Expr* left;
       int    leftCategory;
       char   op[2];        //2 bytes to save operator
       struct Expr* right;
       int    rightCategory;

};
//EXPR_RELATION
struct relationExpr{
       struct Expr* left;
       int    leftCategory;
       char   op[2];        //2 bytes to save operator
       struct Expr* right;
       int    rightCategory;

};

//EXPR_LOGIC
struct logicExpr{
       struct Expr* left;
       int    leftCategory;
       char   op[2];        //2 bytes to save operator
       struct Expr* right;
       int    rightCategory;

};

//EXPR_READINT
struct ReadInteger{
       int value;
};

//EXPR_READLINE
struct ReadLine{
       char line[MAX_TOKEN_SIZE];
};

//EXPR_INTCONST
struct intExpr{
       int value;
};

//EXPR_DOUBLECONST
struct doubleExpr{
       double value;
};

//EXPR_BOOLCONST
struct boolExpr{
       bool value;
};

//EXPR_STRINGCONST
struct stringExpr{
       char line[MAX_TOKEN_SIZE];
};

//EXPR_NULLCONST
struct nullExpr{
       bool value;
};

/* There are kinds of bodyStmt: seqStmt, ifStmt, forStmt, whileStmt, retStmt, breakStmt
 * In the stmts above, all of them contains basic expressions: [VariableDecl], [assignExpr],
 * [opExpr] and function calls: [readInterger], [readLine], [Print].
*/

// STMT_BLOCK
struct bodyStmt{
       int category;
       void* stmtPointer;//varDecl,if,while,for,break,return,print,expr,
       struct bodyStmt* next;
};

// STMT_IF
struct ifStmt{
       int condCategory;
       struct Expr* cond;
       struct bodyStmt* ifFirst;
       struct bodyStmt* elseFirst;
};

// STMT_FOR
struct forStmt{
       int initCategory;
       int condCategory;
       int updateCategory;
       struct Expr* init;
       struct Expr* cond;
       struct Expr* update;
       struct bodyStmt* stmtFirst;
};

// STMT_WHILE
struct whileStmt{
       int condCategory;
       struct Expr* cond;
       struct bodyStmt* stmtFirst;
};

// STMT_RET
struct retStmt{
       int category;
       struct Expr* retExpr;
};

// STMT_BREAK
struct breakStmt{
      struct Expr* beak; 
};

// STMT_PRINT
struct Print{
       int category;
       struct Expr* outputList;
};

// EXPR_CALL
struct callFunc{
       char ident[MAX_TOKEN_SIZE];
       struct Expr* actualsList;
};

/* Declaration includes variable declaration and function declaration.
 * seqId: records the sequential id of a purticular declaration.
 * type:  indicate the type of variable or return type of functions.
 * ident: saves the name of variable or function.
*/

class VariableDecl{
       public:
           VariableDecl(){
               
           };
           VariableDecl(int declType, char*name){
               type = declType;
               strcpy(ident, name);
               next = NULL;
           };
           ~VariableDecl(){
               if(next !=NULL){delete next; next = NULL;}
           };
       public:
           int type;//int, double, bool, string
           char ident[MAX_TOKEN_SIZE];//1000
           struct VariableDecl* next; //only for functional formals' use
};

class FunctionDecl{
       public:
           FunctionDecl(){
      
           };
           FunctionDecl(int declType, char* name){
               type = declType;
               strcpy(ident, name);
               formalStart = NULL;
               stmtFirst   = NULL;
           };
           ~FunctionDecl(){
                if(formalStart != NULL) {delete formalStart; formalStart=NULL;}
                if(stmtFirst   != NULL) {delete stmtFirst; stmtFirst=NULL;}
           };
       public:
           int type;//int, double, bool, string, void
           char ident[MAX_TOKEN_SIZE];//1000
           struct VariableDecl* formalStart;//a list of varaibles
           struct bodyStmt* stmtFirst;
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
          Decl(int cat, int type){
               category = cat;
               if(category == DECL_VAR){
                    declPointer = (void*)malloc(sizeof(VariableDecl));
                    ((struct VariableDecl*)declPointer)->type = type;
                    declType = type;     
               }else if(category == DECL_FUNC){
                    declPointer = (void*)malloc(sizeof(FunctionDecl));
                    ((struct FunctionDecl*)declPointer)->type = type;
                    declType = type;
               }else{
                    declPointer = NULL;
               }
          };
          ~Decl(){
               if(category == DECL_VAR)  {delete (struct VariableDecl*)declPointer; category=0;}
               if(category == DECL_FUNC) {delete (struct FunctionDecl*)declPointer; category=0;}
               if(nextItem != NULL)      {delete (nextItem); nextItem=NULL;}
          }
      public:
          int declType;//int, double, bool, string
          char ident[MAX_TOKEN_SIZE];//1000
          int category; //1: var, 2: func
          void* declPointer;//VariableDecl, FunctionDecl
          struct Decl* nextItem;
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
      }
      public:
           struct Decl *declTableStart;
           int    declTotal;
           int    curStatus;
};

//function declarations
void parser(struct token *token, Program *prog);
