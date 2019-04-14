#include "parser.h"

extern struct token tokenInRow[MAX_TOKEN_IN_ROW];
/* Here defines functions that parser needs.
 *
*/
void* parser(struct token *token, Program *prog, curPointer){
    int token_category = token->category;
    if(prog->declTableStart == NULL){//The first token
         if(token_category == T_Void){
             prog->declTableStart = new Decl(DECL_FUNC, T_Void);
             prog->curStaus = STATUS_FUNC;
             return (void*)(prog->declTableStart->declPointer);
         }else if(token_category == T_Int  || token_category == T_Double ||
                 token_category == T_Bool || token_category == T_String){
 
             prog->declTableStart = new Decl(token_category);
             prog->curStaus = STATUS_DECL;
             return (void*)(prog->declTableStart);
         }else{
             prog->curStaus = STATUS_PROGRAM;
             printf("declaration has no type..\n");
             return false;
         }
    }else{
         switch(prog->curStaus){
                case STATUS_PROGRAM:
                     
                     break;
                case STATUS_DECL:
                     if(token_category == T_Identifier && curPointer->ident[0]=='\0'){
                         strcpy(curPointer->ident,token->token);
                         return curPointer; 
                     }else if(token_category == T_LPara){//func decl
                         FunctionDecl* declFunc  = new FunctionDecl(curPoiner->declType, curPointer->ident);
                         curPointer->declPointer = (void*)declFunc;
                         prog->curStaus = STATUS_FUNC;
                         return declFunc;                
                     }else if(token_category == T_SemiColon){//var decl
                         VariableDecl* declVar = new VariableDecl(curPoiner->declType, curPointer->ident);
                         curPointer->declPointer = (void*)declVar;
                          
                     }else{}//Error
                     break;
                case STATUS_VAR:
                     break;
                case STATUS_FUNC:
                     break;
         }
    }


}





