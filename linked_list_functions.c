#include "kernel_functions.h"
extern list *WaitingList, *ReadyList, *TimerList;

///////READY LIST///////
exception insert_into_readylist(listobj *NewObject){
  listobj *temp = ReadyList->pHead;
  if(NewObject == NULL)
    return FAIL;
  //If list is empty, but for idle node
  if(ReadyList->pHead == ReadyList->pTail){
    ReadyList->pTail->pPrevious = NewObject;
    ReadyList->pHead = NewObject;
    NewObject->pNext = ReadyList->pTail;
  }
  //Find correct insert position
    else if(temp->pTask->Deadline > NewObject->pTask->Deadline){
    NewObject->pNext = temp;
    temp->pPrevious = NewObject;
    ReadyList->pHead = NewObject;
  }
  else{
    while(NewObject->pTask->Deadline > temp->pTask->Deadline){
      temp = temp->pNext;
      if(temp == ReadyList->pTail)
        break;
    }
    //Link new object to list objects
    NewObject->pPrevious = temp->pPrevious;
    NewObject->pNext = temp;
 
    //Link list objects to new object 
    temp->pPrevious->pNext = NewObject;
    temp->pPrevious = NewObject;
  }
  return OK;
}

listobj* extract_from_readylist(){
  listobj* leavingObj = ReadyList->pHead;
  if(ReadyList->pHead == NULL){
  }
  else if(ReadyList->pTail == ReadyList->pHead){
    ReadyList->pHead = NULL;
    ReadyList->pTail = NULL;
  }
  else{
    ReadyList->pHead = ReadyList->pHead->pNext;
    ReadyList->pHead->pPrevious = NULL;
  }
  return leavingObj;
}


void reschedule_readylist(){
  insert_into_readylist(extract_from_readylist());
}


///////WAITING LIST///////
void insert_into_waitinglist(){
  listobj* movingObj = extract_from_readylist();
  movingObj->pNext = NULL;
  movingObj->pPrevious = NULL;
  //Step 1: Add to WaitingList
  //Is the list empty?
  if(WaitingList->pHead == NULL){
    WaitingList->pHead = movingObj;
  }
  //Is there only one node?
  else if(WaitingList->pHead == WaitingList->pTail){
    WaitingList->pHead->pNext = movingObj;
    movingObj->pPrevious = WaitingList->pHead;
  }
  //Put at tail
  else{
    WaitingList->pTail->pNext = movingObj;
    movingObj->pPrevious = WaitingList->pTail;
  }
  //Finalize list linking
  WaitingList->pTail = movingObj;
}

void remove_from_waitinglist(listobj* WaitingObject){
  //Is this the last object in list?
  if(WaitingList->pHead == WaitingList->pTail){
    WaitingList->pHead = WaitingObject->pPrevious;
    WaitingList->pTail = WaitingObject->pPrevious;
  }
  else{
    //If the object is at the head
    if(WaitingObject->pPrevious == NULL){
      WaitingList->pHead = WaitingObject->pNext;
      WaitingList->pHead->pPrevious = NULL;
    }
    //If the object is at the tail
    else if(WaitingObject->pNext == NULL){
      WaitingList->pTail = WaitingObject->pPrevious;
      WaitingList->pTail->pNext = NULL;
    }
    //If not, it's somewhere in "the middle"
    else{  
    WaitingObject->pPrevious->pNext = WaitingObject->pNext;
    WaitingObject->pNext->pPrevious = WaitingObject->pPrevious;  
    }
  }
  
  WaitingObject->pPrevious = NULL;
  WaitingObject->pNext = NULL;
  insert_into_readylist(WaitingObject);
}


///////TIMER LIST///////
void insert_into_timerlist(){
  listobj* movingObj = extract_from_readylist();
  movingObj->pNext = NULL;
  movingObj->pPrevious = NULL;
  //Step 1: Add to TimerList
  //Is the list empty?
  if(TimerList->pHead == NULL){
    TimerList->pHead = movingObj;
  }
  //Is there only one node?
  else if(TimerList->pHead == TimerList->pTail){
    TimerList->pHead->pNext = movingObj;
    movingObj->pPrevious = TimerList->pHead;
  }
  //Put at tail
  else{
    TimerList->pTail->pNext = movingObj;
    movingObj->pPrevious = TimerList->pTail;
  }
  //Finalize list linking
  TimerList->pTail = movingObj;  
}

void remove_from_timerlist(listobj* SleepingObject){
  SleepingObject->nTCnt = 0;
  //Is this the last object in list?
  if(TimerList->pHead == TimerList->pTail){
    TimerList->pHead = NULL;
    TimerList->pTail = NULL;
  }
  else{
    if(SleepingObject->pPrevious == NULL){
      TimerList->pHead = SleepingObject->pNext;
      TimerList->pHead->pPrevious = NULL;
    }
    else if(SleepingObject->pNext == NULL){
      TimerList->pTail = SleepingObject->pPrevious;
      TimerList->pTail->pNext = NULL;
    }
    else{  
    SleepingObject->pPrevious->pNext = SleepingObject->pNext;
    SleepingObject->pNext->pPrevious = SleepingObject->pPrevious;  
    }
  }
  
  SleepingObject->pPrevious = NULL;
  SleepingObject->pNext = NULL;
  insert_into_readylist(SleepingObject);
}