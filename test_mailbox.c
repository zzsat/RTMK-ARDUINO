/*THIS UNIT TEST WILL TEST THE SUPPORTING LIBRARY FOR THE FOLLOWING STRUCTURES:
typedef struct msgobj {
        char            *pData;
        exception       Status;
        struct l_obj    *pBlock;
        struct msgobj   *pPrevious;
        struct msgobj   *pNext;
} msg;

typedef struct {
        msg             *pHead;
        msg             *pTail;
        int             nDataSize;
        int             nMaxMessages;
        int             nMessages;
        int             nBlockedMsg;
} mailbox;
*/

#include "kernel_functions.h"
#include "linked_list_functions.h"
#include "mailbox_functions.h"

exception condition = 1;
mailbox *charMbox; 
mailbox *intMbox; 
mailbox *floatMbox;



//mailbox*        create_mailbox(uint nMessages, uint nDataSize);
exception test_create_mailbox(){
//Assert that creation works as intended for char type mailbox
  charMbox = create_mailbox(3 , sizeof(char));
  if(charMbox == NULL){
    condition = FAIL;
  }
  else if(charMbox->pTail != NULL){
    condition = FAIL;
  }
  else if(charMbox->nDataSize != sizeof(char)){
    condition = FAIL;
  }
  else if(charMbox->nMaxMessages != 3){
    condition = FAIL;
  }
  else if(charMbox->nMessages != 0){
    condition = FAIL;
  }
  else if(charMbox->nBlockedMsg != 0){
    condition = FAIL;
  }
  free(charMbox);
  
////Assert that creation works as intended for int type mailbox 
  intMbox = create_mailbox(10 , sizeof(int));
  if(charMbox == NULL){
    condition = FAIL;
  }
  else if(charMbox->pTail != NULL){
    condition = FAIL;
  }
  else if(charMbox->nDataSize != sizeof(int)){
    condition = FAIL;
  }
  else if(charMbox->nMaxMessages != 10){
    condition = FAIL;
  }
  else if(charMbox->nMessages != 0){
    condition = FAIL;
  }
  else if(charMbox->nBlockedMsg != 0){
    condition = FAIL;
  }
  free(intMbox);
  
////Assert that creation works as intended for float type mailbox 
  mailbox* floatMbox = create_mailbox(0, sizeof(float));
  if(charMbox == NULL){
    condition = FAIL;
  }
  else if(floatMbox->pTail != NULL){
    condition = FAIL;
  }
  else if(floatMbox->nDataSize != sizeof(float)){
    condition = FAIL;
  }
  else if(floatMbox->nMaxMessages != 0){
    condition = FAIL;
  }
  else if(floatMbox->nMessages != 0){
    condition = FAIL;
  }
  else if(floatMbox->nBlockedMsg != 0){
    condition = FAIL;
  }
  free(floatMbox);
  
  return condition;
  
}



//void            add_message(mailbox* mBox, msg* message);
exception test_add_message(){
  //Prepare mailbox for test purposes
  intMbox = create_mailbox(3 , sizeof(int));
  //Prepare message for test purposes
  msg* NewMessage1 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage2 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage3 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage4 = (msg *) calloc(1, sizeof(msg));

  
  //Assert insertion to empty mailbox
  add_message(intMbox, NewMessage1);
  if(intMbox->nMessages != 1){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail->pPrevious != NULL && intMbox->pTail->pNext != NULL){
    condition = FAIL;
  }
  else if(intMbox->pHead->pPrevious != NULL && intMbox->pHead->pNext != NULL){
    condition = FAIL;
  }
  
  //Assert blocked message insertion is illegal
  add_blocked_message(intMbox, NewMessage2);
  if(intMbox->nMessages != 1){
    condition = FAIL;
  }
  else if(intMbox->nBlockedMsg != 0){
    condition = FAIL;
  }
  
  //Assert tail insertion to mailbox with 1 object
  add_message(intMbox, NewMessage2);
  if(intMbox->nMessages != 2){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail != NewMessage2){
    condition = FAIL;
  }
  else if(intMbox->pHead->pNext != intMbox->pTail){
    condition = FAIL;
  }
  else if(intMbox->pTail->pNext != NULL && intMbox->pTail->pPrevious != intMbox->pHead){
    condition = FAIL;
  }
  
  //Assert tail insertion to mailbox with n objects
  add_message(intMbox, NewMessage3);
  if(intMbox->nMessages != 3){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail != NewMessage3){
    condition = FAIL;
  }
  else if(NewMessage2->pNext != intMbox->pTail){
    condition = FAIL;
  }
  else if(NewMessage3->pNext != NULL && intMbox->pTail->pPrevious != NewMessage2){
    condition = FAIL;
  }
  
  
  //Assert full mailbox insertion is illegal
  add_message(intMbox, NewMessage4);
  if(intMbox->nMessages != 3){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  
  /*
  //Assert full mailbox insertion leads to head overwrite
  add_message(intMbox, NewMessage4);
  if(intMbox->nMessages != 3){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail != NewMessage3){
    condition = FAIL;
  }
  else if(NewMessage2->pNext != intMbox->pTail){
    condition = FAIL;
  }
  else if(NewMessage3->pNext != NULL && intMbox->pTail->pPrevious != NewMessage2){
    condition = FAIL;
  }
  */
  free(intMbox);
  free(NewMessage1);
  free(NewMessage2);
  free(NewMessage3);
  free(NewMessage4);
  
  return condition;
}



//void            add_blocked_message(mailbox* mBox, msg* message);
exception test_add_blocked_message(){
  //Prepare mailbox for test purposes
  intMbox = create_mailbox(3 , sizeof(int));
  //Prepare message for test purposes
  msg* NewMessage1 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage2 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage3 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage4 = (msg *) calloc(1, sizeof(msg));

  //Assert insertion to empty mailbox
  add_blocked_message(intMbox, NewMessage1);
  if(intMbox->nBlockedMsg != 1){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail->pPrevious != NULL && intMbox->pTail->pNext != NULL){
    condition = FAIL;
  }
  else if(intMbox->pHead->pPrevious != NULL && intMbox->pHead->pNext != NULL){
    condition = FAIL;
  }
  
  //Assert unblocked message insertion is illegal
  add_message(intMbox, NewMessage2);
  if(intMbox->nBlockedMsg != 1){
    condition = FAIL;
  }
  else if(intMbox->nMessages != 0){
    condition = FAIL;
  }
  
  //Assert tail insertion to mailbox with 1 object
  add_blocked_message(intMbox, NewMessage2);
  if(intMbox->nBlockedMsg != 2){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail != NewMessage2){
    condition = FAIL;
  }
  else if(intMbox->pHead->pNext != intMbox->pTail){
    condition = FAIL;
  }
  else if(intMbox->pTail->pNext != NULL && intMbox->pTail->pPrevious != intMbox->pHead){
    condition = FAIL;
  }
  
  //Assert tail insertion to mailbox with n objects
  add_blocked_message(intMbox, NewMessage3);
  if(intMbox->nBlockedMsg != 3){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail != NewMessage3){
    condition = FAIL;
  }
  else if(NewMessage2->pNext != intMbox->pTail){
    condition = FAIL;
  }
  else if(NewMessage3->pNext != NULL && intMbox->pTail->pPrevious != NewMessage2){
    condition = FAIL;
  }
  
  
  //Assert full mailbox insertion is illegal
  add_blocked_message(intMbox, NewMessage4);
  if(intMbox->nBlockedMsg != 3){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  
  /*
  //Assert full mailbox insertion leads to head overwrite
  add_message(intMbox, NewMessage4);
  if(intMbox->nBlockedMsg != 3){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  else if(intMbox->pTail != NewMessage3){
    condition = FAIL;
  }
  else if(NewMessage2->pNext != intMbox->pTail){
    condition = FAIL;
  }
  else if(NewMessage3->pNext != NULL && intMbox->pTail->pPrevious != NewMessage2){
    condition = FAIL;
  }
  */
  free(intMbox);
  free(NewMessage1);
  free(NewMessage2);
  free(NewMessage3);
  free(NewMessage4);
  
  return condition;
}


//void            remove_message(mailbox* mBox, msg* message);
exception test_remove_message(){
  //Prepare mailbox for test purposes
  intMbox = create_mailbox(3 , sizeof(int));
  //Prepare message for test purposes
  msg* NewMessage1 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage2 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage3 = (msg *) calloc(1, sizeof(msg));
  add_message(intMbox, NewMessage1);
  add_message(intMbox, NewMessage2);
  add_message(intMbox, NewMessage3);
  
  //Assert mailbox size
  if(intMbox->nMessages != 3){
    condition = FAIL;
  }
  if(intMbox->pHead != NewMessage1){
    condition = FAIL;
  }
  if(intMbox->pTail != NewMessage3){
    condition = FAIL;
  }
  
  //Assert head removal works as intended
  remove_message(intMbox, NewMessage1);
  if(intMbox->nMessages != 2){
    condition = FAIL;
  }
  if(intMbox->pHead != NewMessage2){
    condition = FAIL;
  }
  
  //Assert tail removal works as intended
  remove_message(intMbox, NewMessage3);
  if(intMbox->nMessages != 1){
    condition = FAIL;
  }
  if(intMbox->pTail != NewMessage2){
    condition = FAIL;
  }
  else if(intMbox->pHead != NewMessage2){
    condition = FAIL;
  }
  
  //Assert last object removal works as intended
  remove_message(intMbox, NewMessage2);
  if(intMbox->nMessages != 0){
    condition = FAIL;
  }
  if(intMbox->pTail != NULL && intMbox->pHead != NULL){
    condition = FAIL;
  }
  
  free(intMbox);
  return condition;
}

//exception       remove_mailbox(mailbox* mBox);
exception test_remove_mailbox(){
  exception retVal;
  //Assert that removal works as intended for empty mailbox
  mailbox* charMbox = create_mailbox(5 , sizeof(char));
  retVal = remove_mailbox(charMbox);
  if(charMbox == NULL){
    condition = FAIL;
  }
  else if(retVal != OK){
    condition = FAIL;
  }
  
  //Prepare message for test purposes
  mailbox* intMbox = create_mailbox(5 , sizeof(int));
  msg* NewMessage = (msg *) calloc(1, sizeof(msg));
  NewMessage->pData = (char *) calloc(1, sizeof(char));
  add_message(intMbox, NewMessage);
  
  //Assert that removal is illegal for non empty mailbox
  retVal = remove_mailbox(intMbox);
  if(retVal != NOT_EMPTY){
    condition = FAIL;
  }
  
  remove_message(intMbox, NewMessage);
  retVal = remove_mailbox(intMbox);
  
  //Assert that removal is legal for emptied mailbox
  if(retVal != OK){
    condition = FAIL;
  }
  return condition;
}

//int             no_messages( mailbox* mBox );
exception test_no_messages(){
  int retVal1;
  //Prepare mailbox for test purposes
  intMbox = create_mailbox(3 , sizeof(int));
  //Prepare message for test purposes
  msg* NewMessage1 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage2 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage3 = (msg *) calloc(1, sizeof(msg));
  
  //Assert that no_messages() returns 0 for empty mailbox
  retVal1 = no_messages(intMbox);
  if(retVal1 != intMbox->nMessages && retVal1 != 0){
    condition = FAIL;
  }
  
  add_message(intMbox, NewMessage1);
  //Assert that no_messages() returns 1 for mailbox with 1 message
  retVal1 = no_messages(intMbox);
  if(retVal1 != intMbox->nMessages && retVal1 != 1){
    condition = FAIL;
  }
  
  
  add_message(intMbox, NewMessage2);
  //Assert that no_messages() returns n for mailbox with n messages
  retVal1 = no_messages(intMbox);
  if(retVal1 != intMbox->nMessages && retVal1 != 2){
    condition = FAIL;
  }
  
  free(NewMessage1);
  free(NewMessage2);
  free(NewMessage3);
  free(intMbox);
  return condition;
}



//int             no_blocked_messages( mailbox* mBox );
exception test_no_blocked_messages(){
  int retVal1;
  //Prepare mailbox for test purposes
  intMbox = create_mailbox(3 , sizeof(int));
  //Prepare message for test purposes
  msg* NewMessage1 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage2 = (msg *) calloc(1, sizeof(msg));
  msg* NewMessage3 = (msg *) calloc(1, sizeof(msg));
  
  //Assert that no_blocked_messages() returns 0 for empty mailbox
  retVal1 = no_blocked_messages(intMbox);
  if(retVal1 != intMbox->nBlockedMsg && retVal1 != 0){
    condition = FAIL;
  }
  
  add_message(intMbox, NewMessage1);
  //Assert that no_blocked_messages() returns 1 for mailbox with 1 message
  retVal1 = no_blocked_messages(intMbox);
  if(retVal1 != intMbox->nBlockedMsg && retVal1 != 1){
    condition = FAIL;
  }
  
  
  add_message(intMbox, NewMessage2);
  //Assert that no_blocked_messages() returns n for mailbox with n messages
  retVal1 = no_blocked_messages(intMbox);
  if(retVal1 != intMbox->nBlockedMsg && retVal1 != 2){
    condition = FAIL;
  }
  
  free(NewMessage1);
  free(NewMessage2);
  free(NewMessage3);
  free(intMbox);
  return condition;
}

//exception       message_status(mailbox* mBox);
exception test_message_status(){
  int retVal1;
  //Prepare mailbox for test purposes
  intMbox = create_mailbox(3 , sizeof(int));
  //Prepare message for test purposes
  msg* NewMessage1 = (msg *) calloc(1, sizeof(msg));
  NewMessage1->Status = RECEIVER;
  msg* NewMessage2 = (msg *) calloc(1, sizeof(msg));
  NewMessage2->Status = SENDER;
  add_message(intMbox, NewMessage1);
  add_message(intMbox, NewMessage2);
  
  //Assert that RECEIVER message returns -1
  retVal1 = message_status(intMbox);
  if(retVal1 != -1){
    condition = FAIL;
  }
  
  remove_message(intMbox, NewMessage1);
  
  //Assert that SENDER message returns +1
  retVal1 = message_status(intMbox);
  if(retVal1 != +1){
    condition = FAIL;
  }
  remove_message(intMbox, NewMessage2);
  remove_mailbox(intMbox);
  return condition;
}

//int             is_wait_type(msg* message);
exception test_is_wait_type(){
  int retVal1;
  //Prepare message for test purposes
  msg* NewMessage1 = (msg *) calloc(1, sizeof(msg));
  
  //Assert that non bblocked message returns 0
  retVal1 = is_wait_type(NewMessage1);
  if(retVal1 != 0){
    condition = FAIL;
  }
  
  NewMessage1->pBlock = (listobj *) calloc(1, sizeof(listobj));
    
  //Assert that non blocked message returns 1
  retVal1 = is_wait_type(NewMessage1);
  if(retVal1 != 1){
    condition = FAIL;
  }
  free(NewMessage1);
  return condition;
}  
  



void main(){
  condition = test_create_mailbox();
  if(condition == FAIL){
    while(1){}
  }
  
  condition = test_add_message();
  if(condition == FAIL){
    while(1){}
  }
  
  condition = test_add_blocked_message();
  if(condition == FAIL){
    while(1){}
  }
  
  condition = test_remove_message();
  if(condition == FAIL){
    while(1){}
  }
  
  condition = test_no_messages();
  if(condition == FAIL){
    while(1){}
  }
  
  condition = test_no_blocked_messages();
  if(condition == FAIL){
    while(1){}
  }
  
  condition = test_remove_mailbox();
  if(condition == FAIL){
    while(1){}
  }
  
  condition = test_message_status();
  if(condition == FAIL){
    while(1){}
  }
  
  condition = test_is_wait_type();
  if(condition == FAIL){
    while(1){}
  }
  
  
    
  if(condition == OK)
    while(1){}/*Test was cleared successfully*/
}
