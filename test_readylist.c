#include "kernel_functions.h"
#include "linked_list_functions.h"
#include "readylist_functions.h"

///////TEST MAILBOX/////



exception test_readylist_empty_insert_message(){
}

//Assert front insertion works as intended for 1 objects
exception test_readylist_front_insert_message(){

}

//Assert front insertion works as intended for 1+n objects
exception test_readylist_front_insert_n_objects(){
}

exception

//Assert tail insertion works as intended for 1 objects
exception test_readylist_front_tail_insert(){
}

//Assert tail insertion works as intended for 1+n objects
exception test_readylist_front_tail_insert(){
}


int             no_messages( mailbox* mBox );
int             no_blocked_messages( mailbox* mBox );
exception       message_status(mailbox* mBox);
int             is_wait_type(msg* message);
void            add_message(mailbox* mBox, msg* message);
void            add_blocked_message(mailbox* mBox, msg* message);



///////TEST MESSAGES/////
exception test_readylist_middle_insert(){
}

exception test_readylist_remove

exception test_readylist_middle




void main(){
}

