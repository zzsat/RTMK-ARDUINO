#include "kernel_functions.h"

mailbox* create_mailbox(uint nMessages, uint nDataSize){
  mailbox* NewMailBox = (mailbox *) calloc(1, sizeof(mailbox));
  if(NewMailBox != NULL){
    NewMailBox->nDataSize = nDataSize;
    NewMailBox->nMaxMessages = nMessages;
    NewMailBox->nMessages = 0;
    NewMailBox->nBlockedMsg = 0;
    return NewMailBox;
  }
  else
    return NULL;
}

int no_messages( mailbox* mBox ){
  return mBox->nMessages;
}

int no_blocked_messages( mailbox* mBox ){
  return mBox->nBlockedMsg;
}

exception message_status(mailbox* mBox){
    return mBox->pHead->Status;
}

int is_wait_type(msg* message){
  if(message->pBlock != NULL)
    return 1;
  else
    return 0;
}

void add_message(mailbox* mBox, msg* message){
  if(no_blocked_messages(mBox) == 0){
    if(mBox->nMaxMessages > no_messages(mBox)){
      if(no_messages(mBox) == 0){
        mBox->pHead = message;
        mBox->pTail = message;
      }
      else{
        mBox->pTail->pNext = message;
        message->pPrevious = mBox->pTail;
        mBox->pTail = message;
      }
      mBox->nMessages++;
    }
  }
}

void add_blocked_message(mailbox* mBox, msg* message){
  if(no_messages(mBox) == 0){
    if(mBox->nMaxMessages > no_blocked_messages(mBox)){
      if(no_blocked_messages(mBox) == 0){
        mBox->pHead = message;
        mBox->pTail = message;
      }
      else{
        mBox->pTail->pNext = message;
        message->pPrevious = mBox->pTail;
        mBox->pTail = message;
      }
      mBox->nBlockedMsg++;
    }
  }
}

exception remove_mailbox(mailbox* mBox){
  if((no_messages(mBox) == 0) && (no_blocked_messages(mBox) == 0)){
    free(mBox);
    return OK;
  }
  else
    return NOT_EMPTY;
}

void remove_message(mailbox* mBox, msg* message){
  if(mBox->pHead == message && mBox->pTail == message){
    mBox->pHead = NULL;
    mBox->pTail = NULL;
  }
  //If message to be removed is at head
  else if(mBox->pHead == message){
      mBox->pHead = mBox->pHead->pNext;
      //If mailbox now contains only one item
      if(mBox->pHead->pNext == NULL)
        mBox->pTail->pPrevious = NULL;
    }
  //If messages to be removed is at tail
  else if(mBox->pTail == message){
    mBox->pTail = mBox->pTail->pPrevious;
  }
    //If message to be remove is in "the middle"
  else{
    message->pPrevious->pNext = message->pNext;
    message->pNext->pPrevious = message->pPrevious;
  }
  if(mBox->nMessages != 0){
    mBox->nMessages--;
  }  
  else if(mBox->nBlockedMsg !=0){
    mBox->nBlockedMsg--;
  }
  free(message);
}