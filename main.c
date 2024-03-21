/*THIS INTEGRATION TEST WILL PRIMARILY TEST FOR FUNCTIONALITY NOT EXAMINED IN:
test1.c
test2.c
test3.c
WHICH IS:
ASYNCHRONOUS-SYNCHRONOUS INTER-PROCESS COMMUNICATION
TASK SCHEDULING WHEN HANDLING EXPIRED DEADLIES
ASSERTING THAT FLOAT VALUES ARE NOT GARBLED IN INTER-PROCESS COMMUNICATION
what happens with message that was never inserted into mailbox? :S
lost communication!
task will be blocked forever
task creation in run mode
*/

#include "system_sam3x.h"
#include "at91sam3x8.h"

#include "kernel_functions.h"
#include "linked_list_functions.h"
#include "mailbox_functions.h"


unsigned int g0=0, g1=0, g2=0, g3=0, g4 = 0;  
/* gate flags for various stages of unit test */

mailbox *charMbox; 
mailbox *intMbox;
mailbox *floatMbox;
extern list *WaitingList, *ReadyList, *TimerList;



//receive_no_wait unblocks send_wait

void task_body_1(){
  int retVal_t1;
  char varChar_t1;
  int varInt_t1;
  
  //Synchronous receive blocks this task
  retVal_t1 = receive_wait(charMbox, &varChar_t1);
  
  //TEST!
  //Assert that asynchronous send unblocked synchronous receive
  if(retVal_t1 == OK){
    if(varChar_t1 != 'a'){
      g1 = FAIL;
      while(1) {}
    }
  }
  
  //Blocks this task again
  retVal_t1 = receive_wait(intMbox, &varInt_t1);
  //TEST!
  //Assert that deadline expired while task was blocked
  if(retVal_t1 != DEADLINE_REACHED){
    g1 = FAIL;
    while(1) {}
  }
  //TEST!
  //Assert that deadline expired "on time" :)
  if(ticks() != 1000){
    g1 = FAIL;
    while(1) {}
  }
  
  //TEST!
  //Assert that after expired deadline, intMbox contains 1 message
  retVal_t1 = no_blocked_messages(intMbox);
  if(retVal_t1 != 1){
    g1 = FAIL;
    while(1) {}
  }
  
  retVal_t1 = wait(4000);
  if(retVal_t1 != DEADLINE_REACHED){
    g1 = FAIL;
    while(1) {}
  }
  g1 = OK;
  terminate();
}

void task_body_2(){
  int retVal_t2;
  char varChar_t2;
  varChar_t2 = 'a';
  
  //Asynchronous send should unblock synchronous receive
  retVal_t2 = send_no_wait(charMbox, &varChar_t2);
  if (retVal_t2 != OK){
    g2 = FAIL;
    while(1) {}
  }
  
  varChar_t2 = 'b';
  //Synchronous send blocks this task
  retVal_t2 = send_wait(charMbox, &varChar_t2);
  
  //TEST!
  //Assert that asynchronous receive unblocked synchronous send
  if (retVal_t2 != OK){
    g2 = FAIL;
    while(1) {}
  }
  

  retVal_t2 = receive_wait(intMbox, &varChar_t2);
  //TEST!
  //Assert that deadline expired while task was blocked
  if(retVal_t2 != DEADLINE_REACHED){
    g2 = FAIL;
    while(1) {}
  }
  
  //TEST!
  //Assert that deadline expired "on time" :)
  if(ticks() != 2000){
    g2 = FAIL;
    while(1) {}
  }
  
  //TEST!
  //Assert that after expired deadline, intMbox contains 0 messages
  retVal_t2 = no_blocked_messages(intMbox);
  if(retVal_t2 != 0){
    g2 = FAIL;
    while(1) {}
  }
  
  retVal_t2 = wait(4000);
  if(retVal_t2 != DEADLINE_REACHED){
    g2 = FAIL;
    while(1) {}
  }
  g2 = OK;
  terminate();
}

void task_body_3(){
  int retVal_t3;
  char varChar_t3;
  float varFloat_t3;
  
  //Asyncrhonous receive should unblock synchronous send
  retVal_t3 = receive_no_wait(charMbox, &varChar_t3);
  if (retVal_t3 == OK){
    if(varChar_t3 != 'b'){
      g3 = FAIL;
      while(1) {}
    }
  }
  
  //TEST!
  //Assert that charMbox is properly emptied
  retVal_t3 = no_blocked_messages(charMbox);
  if(retVal_t3 != 0){
    g3 = FAIL;
    while(1){}
  }
  
  //TEST!
  //Assert that intMbox contains two blocked messages
  retVal_t3 = no_blocked_messages(intMbox);
  if(retVal_t3 != 2){
    g3 = FAIL;
    while(1){}
  }
  
  wait(100);
  
  varFloat_t3 = 3.721f;
  //send_wait should properly handle float value
  retVal_t3 = send_wait(floatMbox, &varFloat_t3);
  if(retVal_t3 != OK){
    g3 = FAIL;
    while(1){}
  }

  varFloat_t3 = 1.62f;
  //send_no_wait should properly handle float value
  retVal_t3 = send_no_wait(floatMbox, &varFloat_t3);
  if(retVal_t3 != OK){
    g3 = FAIL;
    while(1){}
  }
  
  
  //wait(2700);
  g3 = OK;
  terminate();
}

void task_body_4(){
  float retVal_t4;
  float varFloat_t4;

  wait(200);
  
  retVal_t4 = receive_no_wait(floatMbox, &varFloat_t4);
  //Assert that float value is received correctly vy receive_no_wait
  if(retVal_t4 == OK){
    if(varFloat_t4 != 3.721f){
    g4 = FAIL;
    while(1){}
    }
  }
  
  retVal_t4 = receive_wait(floatMbox, &varFloat_t4);
  //Assert that float value is received correctly vy receive_wait
  if(retVal_t4 == OK){
    if(varFloat_t4 != 1.62f){
      g4 = FAIL;
      while(1){}
    }
  }
  
  wait(4000);
  g4 = OK;
  terminate();
}

void task_body_5(){
  wait(10000);
  if(g0 == OK && g1 == OK && g2 == OK && g3 == OK && g4 == OK){
    while(1){} /* This test has been cleared successfully */
  }
  terminate();
}

void main()
{
  int retVal;
  SystemInit(); 
  SysTick_Config(100000); 
  SCB->SHP[((uint32_t)(SysTick_IRQn) & 0xF)-4] =  (0xE0);      
  isr_off();
  init_kernel();
  
  charMbox = create_mailbox(4, sizeof(char));
  intMbox = create_mailbox(4, sizeof(int));
  floatMbox = create_mailbox(3, sizeof(float));

  retVal = create_task(task_body_1, 1000);
  if(retVal !=  OK){
    g0 = FAIL;
    while(1){}
  }
  
  retVal = create_task(task_body_2, 2000);
  if(retVal !=  OK){
    g0 = FAIL;
    while(1){}
  }
  
  retVal = create_task(task_body_3, 3000);
  if(retVal !=  OK){
    g0 = FAIL;
    while(1){}
  }
  
  retVal = create_task(task_body_4, 4000);
  if(retVal !=  OK){
    g0 = FAIL;
    while(1){}
  }
  
  retVal = create_task(task_body_5, 800000);
  if(retVal !=  OK){
    g0 = FAIL;
    while(1){}
  }
  g0 = OK;
  run();
}
