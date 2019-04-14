#include "parser.h"

extern struct token tokenInRow[MAX_TOKEN_IN_ROW];
/* Here defines functions that parser needs.
 *
*/


bool parserFuncFormals(curPointer->declPointer->formalStart){

     return true;
}
bool parserFuncBody(curPointer->declPointer->stmtFirst){

     return true;
}



void* parser(struct token *token, Program *prog, void* pointer){
    int token_category = token->category;
    if(prog->declTableStart == NULL){//The first token
         if(token_category == T_Int  || token_category == T_Double || token_category == T_Bool ||
                                        token_category == T_String || token_category == T_Void){
 
             prog->declTableStart   = new Decl(token_category);
             prog->declTableCurrent = prog->declTableStart;
             prog->curStatus = STATUS_DECL;
             return (void*)(prog->declTableCurrent);
         }else{
             prog->curStatus = STATUS_PROGRAM;
             printf("declaration has no type..\n");
             return false;
         }
    }else{
         switch(prog->curStatus){
             case STATUS_PROGRAM:
                  {
                     Decl* curPointer=prog->declTableCurrent;
                     if(token_category == T_Int || token_category == T_Double || token_category == T_Bool
                                               || token_category == T_String || token_category == T_Void){
 
                          curPointer->nextItem = new Decl(token_category);//nextItem should not be 0
                          prog->declTableCurrent = curPointer->nextItem;
                          prog->curStatus = STATUS_DECL;
                          return (void*)(prog->declTableCurrent);
                     }else{
                          prog->curStatus = STATUS_PROGRAM;
                          printf("declaration has no type..\n");
                          return false;
                     }
                     break;
                  }
             case STATUS_DECL:
                  {
                     Decl* curPointer=pointer;
                     if(token_category == T_Identifier && curPointer->ident[0]=='\0'){
                           strcpy(curPointer->ident,token->token);
                           return (void*)curPointer; 
                     }else if(token_category == T_LPara){//func decl
                           FunctionDecl* declFunc  = new FunctionDecl(curPointer->declType, curPointer->ident);
                           curPointer->declPointer = (void*)declFunc;
                           prog->curStatus = STATUS_FORMAL;
                           return (void*)(curPoiner->declPointer);//point to Func 
                     }else if(token_category == T_SemiColon && curPointer->declType!=T_Void){
                           VariableDecl* declVar = new VariableDecl(curPointer->declType, curPointer->ident);
                           curPointer->declPointer = (void*)declVar;
                           curPointer->category    = DECL_VAR;
                           prog->curStatus = STATUS_PROGRAM;
                           return (void*)(curPointer->declPointer);//point to Var 
                     }else{
                           printf("Error occurs in Declaration..\n");
                           return false;
                     }//Error
                     break;
                  }
             case STATUS_FORMAL:
                  {
                     FunctionDecl* curPointer=(FunctionDecl*)pointer;
                     if(token_category == T_RPara){//formal end, body stmt start.
                          printf("Parse Formal End..\n");
                          prog->curStatus = STATUS_FUNC;
                     }else{//parsing formal varaibles passed to a function
                          if(token_category == T_Int || token_category == T_Double
                             || token_category == T_Bool || token_category == T_String)
                          {
                              curPointer->formalStart = new Decl(token_category);//nextItem
                              prog->curStatus = STATUS_FORMAL_DECL;
                              return (void*)(curPointer->formalStart);
                          }
                          if(parserFuncFormals(curPointer->declPointer->formalStart)==false){
                              printf("Error occurs in formal parsing..\n");
                              return false;
                          }
                          curPointer->category = STATUS_FUNC;
                     }
                     return (void*)curPointer;
                     break;
                  }
             case STATUS_FUNC:
                  {
                     if(token_category == T_LCurvePara){//body starts
                          if(parserFuncBody(curPointer->declPointer->stmtFirst)==true){
                              //no errors, func end.
                              prog->curStatus = STATUS_PROGRAM;
                              return (void*)curPointer;
                          }else{//error occurs in body parsing process.
                              printf("Error occurs in body parsing..\n");
                              return false;
                          }
                          
                     }else{
                          printf("Error occurs in STATUS_FUNC..\n");
                     }
                     printf("In STATUS_FUNC..\n");
                     break;
                  }
             default:
                       break;
         }
    }

    return 0;
}





