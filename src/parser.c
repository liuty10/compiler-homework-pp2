#include "parser.h"

extern struct token tokenInRow[MAX_TOKEN_IN_ROW];
/* Here defines functions that parser needs.
 *
*/
void* parser(struct token *token, Program *prog, void* pointer){
    int token_category = token->category;
    if(prog->declTableStart == NULL){//The first token
         if(token_category == T_Void){
             prog->declTableStart = new Decl(DECL_FUNC, T_Void);
             prog->curStatus = STATUS_FUNC;
             return (void*)(prog->declTableStart->declPointer);
         }else if(token_category == T_Int  || token_category == T_Double ||
                 token_category == T_Bool || token_category == T_String){
 
             prog->declTableStart = new Decl(token_category);
             prog->curStatus = STATUS_DECL;
             return (void*)(prog->declTableStart);
         }else{
             prog->curStatus = STATUS_PROGRAM;
             printf("declaration has no type..\n");
             return false;
         }
    }else{
         switch(prog->curStatus){
                case STATUS_PROGRAM:
                      
                     break;
                case STATUS_DECL:
                   {
                     Decl* curPointer=(Decl*)pointer;
                     if(token_category == T_Identifier && curPointer->ident[0]=='\0'){
                         strcpy(curPointer->ident,token->token);
                         return (void*)curPointer; 
                     }else if(token_category == T_LPara){//func decl
                         FunctionDecl* declFunc  = new FunctionDecl(curPointer->declType, curPointer->ident);
                         curPointer->declPointer = (void*)declFunc;
                         prog->curStatus = STATUS_FUNC;
                         return (void*)declFunc;                
                     }else if(token_category == T_SemiColon){//var decl
                         VariableDecl* declVar = new VariableDecl(curPointer->declType, curPointer->ident);
                         curPointer->declPointer = (void*)declVar;
                         curPointer->category    = DECL_VAR;
                         prog->curStatus = STATUS_PROGRAM;
                         return (void*)declVar; 
                     }else{
                         printf("Error occurs in Declaration..\n");
                         return false;
                     }//Error
                     break;
                   }
                case STATUS_VAR:
                     printf("In STATUS_VAR..\n");
                     break;
                case STATUS_FUNC:
                     printf("In STATUS_FUNC..\n");
                     break;
                default:
                       break;
         }
    }

    return 0;
}





