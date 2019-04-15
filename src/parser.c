#include "parser.h"

extern struct token tokenInRow[MAX_TOKEN_IN_ROW];
/* Here defines functions that parser needs.
 *
*/

int formal_type_flag = 0;
int left_curve_para = 0;
int parse_func_flag = 0;

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
                     Decl* curPointer=(Decl*)pointer;
                     if(token_category == T_Identifier && curPointer->ident[0]=='\0'){
                           strcpy(curPointer->ident,token->token);
                           return (void*)curPointer; 
                     }else if(token_category == T_LPara){//func decl
                           FunctionDecl* declFunc  = new FunctionDecl(curPointer->declType, curPointer->ident);
                           curPointer->declPointer = (void*)declFunc;
                           prog->curStatus = STATUS_FORMAL;
                           prog->declTableCurrent->category = DECL_FUNC;
                           formal_type_flag = 1;
                           return (void*)(&(declFunc->formal));//point to Func 
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
                     VariableDecl** curPointer = (VariableDecl**)pointer;
                     if(token_category == T_RPara){//formal end, body stmt start.
                          if(formal_type_flag == 1 || formal_type_flag == 3){
                              printf("Parse Formal End..\n");
                              prog->curStatus = STATUS_FUNCSTART;
                              formal_type_flag = 0;
                              return (void*)(prog->declTableCurrent);
                          }else{
                              printf("Error occurs in formal parsing, expect ident..\n");
                              return false;
                          }
                     }else if(token_category == T_Int || token_category == T_Double
                             || token_category == T_Bool || token_category == T_String){

                              if(formal_type_flag == 1){//just start, expect type
                                  *curPointer = new VariableDecl(token_category);//nextItem
                                  formal_type_flag = 2;//expect ident next time.
                                  return (void*)(curPointer);
                              }else{
                                  printf("Error occurs in formal parsing, expect ident..\n");
                                  return false;
                              }
                     }else if(token_category == T_Identifier){
                              if(formal_type_flag == 2){
                                   strcpy((*curPointer)->ident, token->token);
                                   formal_type_flag = 3;//expect , or ) next time.
                                   return (void*)curPointer;
                              }else{
                                  printf("Error occurs in formal parsing,expect type,comma,RP..\n");
                                  return false;
                              }
                     }else if(token_category == T_Comma){
                               if(formal_type_flag == 3){
                                   //should be good.
                                   formal_type_flag = 1; //start a new para
                                   return (void*)(&((*curPointer)->formal));
                               }else{
                                  printf("Error occurs in formal parsing,flag is not 2..\n");
                                  return false;
                               }
                     }else{
                               printf("Error occurs in formal parsing, bad token..\n");
                               return false;
                     }
                     break;
                  }
             case STATUS_FUNCSTART:
                  {  
                         if(token_category == T_LCurvePara){//body starts
                              parse_func_flag = 1;
                              left_curve_para = 1;
                              prog->curStatus = STATUS_FUNCBODY;
                              return (void*)pointer;//prog->declTableCurrent
                         }else{
                              printf("Error occurs in STATUS_FUNC..missing {\n");
                              return false;
                         }
                         break;
                  }
             case STATUS_FUNCBODY:
                  {
                       if(token_category == T_RCurvePara){
                           if(left_curve_para == 1){
                                left_curve_para = 0;
                                parse_func_flag = 0;
                                prog->curStatus = STATUS_PROGRAM;
                                return (void*)(prog->declTableCurrent);
                           }else{
                                left_curve_para -=1;
                                printf("function body do nothing currently.\n");
                                return pointer;//do nothing 
                           }
                       }else{
                           printf("function body do nothing currently.\n");
                           return pointer;//do nothing currently.
                       }

                  }
             default:
                       break;
         }
    }

    return 0;
}





