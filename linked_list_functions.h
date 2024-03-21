
//READY LIST
exception       insert_into_readylist(listobj *NewObject);
listobj*        extract_from_readylist();
void            reschedule_readylist();

//WAITING LIST
void            insert_into_waitinglist();
void            remove_from_waitinglist(listobj* WaitingObject);

//TIMER LIST
void            insert_into_timerlist();
void            remove_from_timerlist(listobj* SleepingObject);
