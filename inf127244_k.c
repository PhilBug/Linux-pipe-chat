#include "inf127244_h.h"
msgbuf msg_buf;
int clientID, serverID;
bool exitProgram = false;
char usr_input[30];

void set_msg(long type, char* message) {
  msg_buf.mtype = type;
  msg_buf.senderID = clientID;
  strcpy(msg_buf.message, message);
}

void send_msg(int to, char* message) {   //wyslij wiadomosc
    set_msg(1, message);
    msgsnd(to, &msg_buf, sizeof(msg_buf) - sizeof(long), 0);
}

void receive_msg() {
  msgrcv(clientID, &msg_buf, sizeof(msg_buf) - sizeof(long), 1, 0);
}

void get_server_message() {
  receive_msg();
  printf("%s", msg_buf.message);
}


void login() {
  send_msg(serverID, "123connectedxx1");
}

void my_fgets(char* usr_input) {
  fgets(usr_input, 200, stdin);
  usr_input[strcspn(usr_input, "\n\r")] = 0;
}

void user_interaction() {
    get_server_message();
    my_fgets(usr_input);
    send_msg(serverID, usr_input);
}

void receive_usr_msg() {
  msgrcv(clientID, &msg_buf, sizeof(msg_buf) - sizeof(long), 10, IPC_NOWAIT);
  if( !strcmp(msg_buf.message, "read") )
    printf("no new messages\n");
  else
    printf("%s\n", msg_buf.message);
}

int main(int argc, char *argv[]) {
  clientID = msgget(IPC_PRIVATE, IPC_CREAT | 0644); //stworzenie kolejki klienta
  serverID = msgget(222, IPC_CREAT | 0644);
  char *buff = NULL;
  login();
  while ( !exitProgram ) {
    user_interaction();
    buff = strtok(msg_buf.message, " ");
    if( !strcmp(msg_buf.message, "") ||  !strcmp(msg_buf.message, " ") )
      continue;
    if ( !strcmp(buff, "exit") )
      exitProgram = true;
    if ( !strcmp(msg_buf.message, "read") )
      receive_usr_msg();
  }
  send_msg(serverID, "xxx123removeipc321xxx");
  get_server_message();
  printf("Goodbye!\n");
  return 0;

}
