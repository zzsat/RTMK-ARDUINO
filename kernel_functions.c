#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "kernel_functions.h"

int Ticks;
int KernelMode;
TCB *PreviousTask, *NextTask; /* Pointers to previous and next running tasks */
list *ReadyList, *WaitingList, *TimerList;

void idle() { while (1){};}

exception init_kernel()
{
    Ticks = 0;
    
    //Create necessary data structures
    ReadyList = (list *) calloc(1, sizeof(list));
    WaitingList = (list *) calloc(1, sizeof(list));
    TimerList = (list *) calloc(1, sizeof(list));

    if(create_task(idle, -1) != OK)
      return FAIL;
    //Set the kernel in INIT mode
    KernelMode = INIT;
    return OK;
}
void run(void)
{
  Ticks = 0;
  KernelMode = RUNNING;
  //Set NextTask to equal TCB of the next task to be loaded
  NextTask = ReadyList->pHead->pTask;
  LoadContext_In_Run();
}

//TASK CREATION
exception create_task(void (*task_body)(), uint deadline)
{
  //Allocate memory for TCB & list object
  listobj *NewObject;
  NewObject = (listobj *) calloc(1, sizeof(listobj));
  TCB *new_tcb;
  new_tcb = (TCB *) calloc (1, sizeof(TCB));
  new_tcb->PC = task_body;
  
  new_tcb->SPSR = 0x21000000;
  new_tcb->Deadline = deadline+ticks();
  new_tcb->StackSeg [STACK_SIZE - 2] = 0x21000000;
  new_tcb->StackSeg [STACK_SIZE - 3] = (unsigned int) task_body;
  new_tcb->SP = &(new_tcb->StackSeg [STACK_SIZE - 9]);
  
  NewObject->pTask = new_tcb;
  NewObject->pPrevious = NULL;
  NewObject->pNext = NULL;
  

  //Set TCB's SP to point to the correct cell in stack segment ??
  //new_tcb->SP = &new_tcb->StackSeg[0];
  //NewTask->PC = NewTask->SP + (unsigned int) 24; //??
  //NewTask->SPSR = NewTask->SP + (unsigned int) 28; //??
  if(KernelMode==INIT)
  {
    if(ReadyList->pHead == NULL){      
      ReadyList->pHead = NewObject;
      ReadyList->pTail = ReadyList->pHead;
    }
    else{
      if(insert_into_readylist(NewObject) != OK)
        return FAIL;
    }
  }
  else if(KernelMode==RUNNING)
  {
    isr_off();
    PreviousTask = ReadyList->pHead->pTask;
    if(insert_into_readylist(NewObject) != OK)
      return FAIL;
    NextTask = ReadyList->pHead->pTask;
    SwitchContext();
  }
  return OK;
}

void terminate(void)
{
  //Disable interrupts
  isr_off();
  //Remove running task from ReadyList
  listobj* leavingObj = (listobj*) extract_from_readylist();
  
  //Set NextTask to equal TCB of the next Task
  NextTask = ReadyList->pHead->pTask;
  //Switch to process stack of next task to be loaded
  switch_to_stack_of_next_task();
  //Remove data structures of task being terminated
  free(leavingObj->pTask);
  free(leavingObj->pMessage);
  free(leavingObj);
  //Load context
  LoadContext_In_Terminate();
}




//////INTER-PROCESS COMMUNICATION/////
exception send_wait( mailbox* mBox, void* pData ){
  isr_off();

  //Is receiving task waiting?
  if(message_status(mBox) == RECEIVER){
    //Copy senders data (NextTask) to Receivers data
    memcpy(mBox->pHead->pData, pData , mBox->nDataSize);
    //Remove receivers message struct from the mailbox
    listobj* WaitingObject = mBox->pHead->pBlock;
    remove_message(mBox, mBox->pHead);
    //Update PreviousTask
    PreviousTask = ReadyList->pHead->pTask;
    //Insert receiving task in ReadyList
    remove_from_waitinglist(WaitingObject);
    //Update NextTask
    NextTask = ReadyList->pHead->pTask;
  }
  else{
    //Allocate a Message structure Set data pointer
    msg *NewMessage = (msg *) calloc(1, sizeof(msg));
    NewMessage->Status = SENDER;
    //Set data pointer
    NewMessage->pData = (char*) pData;
    //Tihs message is blocked
    NewMessage->pBlock = ReadyList->pHead;
    ReadyList->pHead->pMessage = NewMessage;
    
    //Add Message to the mailbox
    add_blocked_message(mBox, NewMessage);
    
    //Update PreviousTask
    PreviousTask = ReadyList->pHead->pTask;
    //Move sending task from ReadyList to WaitingList
    insert_into_waitinglist();
    //Update NewTask
    NextTask = ReadyList->pHead->pTask;
  }
  
  SwitchContext();
  //If deadline is reached:
  if(ReadyList->pHead->pTask->Deadline <= ticks()){
    isr_off();
    remove_message(mBox, ReadyList->pHead->pMessage);
    isr_on();
    return DEADLINE_REACHED;
  }
  else
    return OK;
}      
      
exception receive_wait( mailbox* mBox, void* pData){
  //Disable interrupt
  isr_off();
  //If send message is waiting:
  if(message_status(mBox) == SENDER){
    //Copy sender's data to receiving task's data area
    memcpy(pData, mBox->pHead->pData, mBox->nDataSize);
    //Remove sending task's Message from mailbox
    msg *temp = mBox->pHead;
    remove_message(mBox, mBox->pHead);
    //If message was of wait type:
    if(is_wait_type(temp) == 1){
      listobj* WaitingObject = temp->pBlock;
      //Update PreviousTask
      PreviousTask = ReadyList->pHead->pTask;
      //Move sending task to ReadyList
      remove_from_waitinglist(WaitingObject);
      //Update NewTask
      NextTask = ReadyList->pHead->pTask;
    }
    else
      //Free senders data area (and message?)
      //remove first message
      free(temp->pData);
       
    free(temp);
  }
  else{
      //Allocate a Message structure
      msg* NewMessage = (msg *) calloc(1, sizeof(msg));
      NewMessage->pData = (char *) pData;
      NewMessage->Status = RECEIVER;
      //This message is blocked
      NewMessage->pBlock = ReadyList->pHead;
      ReadyList->pHead->pMessage = NewMessage;
      
      //Add Message to the mailbox
      add_blocked_message(mBox, NewMessage);
      
      //Update PreviousTask
      PreviousTask = ReadyList->pHead->pTask;
      //Move sending task from ReadyList to WaitingList
      insert_into_waitinglist();
      //Update NewTask
      NextTask = ReadyList->pHead->pTask;
    }
  
   SwitchContext();
  //If deadline is reached:
  if(ReadyList->pHead->pTask->Deadline <= ticks()){
    isr_off();
    //Remove receive message
    remove_message(mBox, ReadyList->pHead->pMessage);
    isr_on();
    return DEADLINE_REACHED;
  }
  else
    return OK;
}

exception send_no_wait( mailbox* mBox, void* pData ){
  isr_off();
  
  //If receiving task is waiting:
  if(message_status(mBox) == RECEIVER){
    listobj* WaitingObject = mBox->pHead->pBlock;
    //Copy sender's data to receiving task's data area
    memcpy(WaitingObject->pMessage->pData, pData , mBox->nDataSize);
    //Remove receiving task's Message struct from the mailbox
    remove_message(mBox, mBox->pHead);
    //Update PreviousTask
    PreviousTask = ReadyList->pHead->pTask;
    //Move receiving task to ReadyList
    remove_from_waitinglist(WaitingObject);
    //Update NewTask
    NextTask = ReadyList->pHead->pTask;
    SwitchContext();
  }
  else{
      //Allocate a Message structure
      msg* NewMessage = (msg *) calloc(1, sizeof(msg));
      NewMessage->pData = (char *) calloc(1, mBox->nDataSize);
      NewMessage->Status = SENDER;
      //Copy data to the message
      memcpy(NewMessage->pData, pData , mBox->nDataSize);
      //If Mailbox is full, remove oldest message
      if(no_messages(mBox) == mBox->nMaxMessages)
        remove_message(mBox, mBox->pHead);
      add_message(mBox, NewMessage);
  }
  return OK;
  
}
     
exception receive_no_wait( mailbox* mBox, void* pData){
  isr_off();
  //If send message is waiting:
  if(message_status(mBox) == SENDER){
    //Copy sender's data to receiving data area
    memcpy(pData, mBox->pHead->pData , mBox->nDataSize);
    //Remove sending task's message struct from the mailbox
    msg* temp = mBox->pHead;
    remove_message(mBox, mBox->pHead);
    //If the message was of wait type:
    if(is_wait_type(temp) == 1){
      listobj* WaitingObject = temp->pBlock;
      //Update PreviousTask
      PreviousTask = ReadyList->pHead->pTask;
      //Move sending task from ReadyList to WaitingList
      remove_from_waitinglist(WaitingObject);
      //Update NewTask
      NextTask = ReadyList->pHead->pTask;
      SwitchContext();
    }
    else
      //Free sender's data area
      free(temp->pData);
       
    free(temp);
  }
  else
    return FAIL;
  return OK;
}



//TIMING FUNCTIONS
exception wait( uint nTicks ){
  isr_off();
  PreviousTask = ReadyList->pHead->pTask;
  //Move sending task from ReadyList to WaitingList
  ReadyList->pHead->nTCnt = nTicks+ticks();
  insert_into_timerlist();
  //Update NewTask
  NextTask = ReadyList->pHead->pTask;
  //Switch context
  SwitchContext();
  
  //If deadline is reached:
  if(ReadyList->pHead->pTask->Deadline <= ticks()){
    return DEADLINE_REACHED;
  }
  else
    return OK;
}

void set_ticks( uint nTicks ){
  Ticks = nTicks;
}

uint ticks( void ){
  return Ticks;
}

uint deadline( void ){
  return ReadyList->pHead->pTask->Deadline;
}

void set_deadline( uint deadline ){
  isr_off();
  //Update deadline in TCB
  ReadyList->pHead->pTask->Deadline = deadline+ticks();
  //Update PreviousTask
  PreviousTask = ReadyList->pHead->pTask;
  //Reschedule ReadyList
  reschedule_readylist();
  //Update NextTask
  NextTask = ReadyList->pHead->pTask;
  //Switch context
  SwitchContext();
}

void TimerInt(void){
  set_ticks(ticks()+1);
  PreviousTask = ReadyList->pHead->pTask;
  //Check the TimerList
  listobj *SleepingObject = TimerList->pHead;
  while(SleepingObject != NULL){
      if((SleepingObject->nTCnt <= ticks()) || SleepingObject->pTask->Deadline <= ticks()){
        remove_from_timerlist(SleepingObject);
        SleepingObject = TimerList->pHead;
      }
      else
        SleepingObject = SleepingObject->pNext;
  }
  
  //Check the WaitingList
  listobj *WaitingObject = WaitingList->pHead;
  while(WaitingObject != NULL){
      if(WaitingObject->pTask->Deadline <= ticks()){
        remove_from_waitinglist(WaitingObject);
        WaitingObject = WaitingList->pHead;
      }
      else
        WaitingObject = WaitingObject->pNext;
  }
  NextTask = ReadyList->pHead->pTask;
}
