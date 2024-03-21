
//MAILBOX FUNCTIONS
mailbox*        create_mailbox(uint nMessages, uint nDataSize);
int             no_messages( mailbox* mBox );
int             no_blocked_messages( mailbox* mBox );
exception       message_status(mailbox* mBox);
int             is_wait_type(msg* message);
void            add_message(mailbox* mBox, msg* message);
void            add_blocked_message(mailbox* mBox, msg* message);
exception       remove_mailbox(mailbox* mBox);
void            remove_message(mailbox* mBox, msg* message);
