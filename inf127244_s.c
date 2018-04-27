#include "inf127244_h.h"
user users[9];
group groups[5];
msgbuf msg_buf;
int serverID;
bool exitProgram = false;
int i,j,k; //iteratory

int load_file(const char *path) {
  FILE *fp;
  size_t len = 0;
  int number, i;
  char *line, *tempChar;
  fp = fopen(path, "r");
  if (fp == NULL) {
    printf("error - cannot load file.\n");
    exit(EXIT_FAILURE);
  }

  getline(&line, &len, fp);
  number = atoi(line);
  for (i = 0; i < number; i++) {
    getline(&line, &len, fp);
    tempChar = strtok(line, " \n");
    strcpy(users[i].name, tempChar);
    tempChar = strtok(NULL, " \n");
    strcpy(users[i].password, tempChar);
  }
  getline(&line, &len, fp);
  number = atoi(line);
  for (i = 0; i < number; i++) {
    getline(&line, &len, fp);
    tempChar = strtok(line, " \n");
    strcpy(groups[i].name, tempChar);
    groups[i].counter = 0;
  }
  fclose(fp);
  return 0;
}

void print_loaded() {
  int i;
  for(i = 0; i < 9; i++) {
    printf("%s : %s\n", users[i].name, users[i].password);
  }
  for(i = 0; i < 3; i++) {
    printf("%s\n", groups[i].name);
  }
}

void set_msg(long type, const char* message) {
  msg_buf.mtype = type;
  strcpy(msg_buf.message, message);
}

void send_msg(int to, const char* message) {
    set_msg(1, message);
    msgsnd(to, &msg_buf, sizeof(msg_buf) - sizeof(long), 0);
}

void send_msg_from_user(int to, const char* message) {  //wiadomosci od uzytkownika typ 10, od serwera 1
    set_msg(10, message);
    msgsnd(to, &msg_buf, sizeof(msg_buf) - sizeof(long), 0); //nowait?
}

bool is_logged(int id) {
  for(i = 0; i < 9; i++) {
    if( id == users[i].userID )
      if( users[i].isLogged ) {
        return true;
      }
  }
  return false;
}

void user_login(int id) {
  char buff[100], *username, *password;
  int tempID;
  bool loginSucces = false;
  //printf("entered user_login function\n");
  if( is_logged(id) ) {
    send_msg(id, "you have to logout first\n");
    return;
  }
    if( !strcmp(msg_buf.message, "") ) {
    send_msg(id, "wrong username or password. Type 'login username password' to try again\n");
    return;
  }
  tempID = id;
  strcpy(buff, msg_buf.message);
  username = strtok(buff," ");
  username = strtok(NULL," ");
  password = strtok(NULL," ");
  if( password == NULL ) {
    send_msg(id, "wrong username or password. Type login 'username' 'password' to try again\n");
    return;
  }
  for(i = 0; i < 9; i++) {
    if( !strcmp(username, users[i].name) )
      if( !strcmp(password, users[i].password) ) {
        if( !users[i].isLogged ) {
          users[i].isLogged = true;
          users[i].userID = tempID;
          loginSucces = true;
          char temp[50];
          sprintf(temp, "Nice to see you %s ! Have a nice day :)\n", users[i].name);
          printf("user %s has logged in\n", users[i].name);
          send_msg(id, temp);
        }
        else {
          char temp[50];
          sprintf(temp, "user %s is already logged\n", users[i].name);
          send_msg(id, temp);
          return;
        }
      }
  }
  if( !loginSucces )
    send_msg(id, "wrong username or password. Type login 'username' 'password' to try again\n");
}

void display_help(int id) {
  if( is_logged(id) )
    send_msg(id, "avaliable commands:\n1. write 'receiver username' 'content'\n2. read\n3. online\n4. showgroups\n5. joingroup 'groupname'\n6. showmembers 'groupname'\n7. writegroup 'groupname'\n8. leavegroup 'groupname'\n9. logout\n10. exit\n");
  else
    send_msg(id, "avaliable commands:\n1. login 'username' 'password'\n2. exit\n");
}

void msg_user(int id) {
  char *buff = NULL, temp[300] = "", temp1[300] = "", name[100] = "", message[400] = "", sender[100] = "";
  time_t currentTime;

  if( !strcmp(msg_buf.message, "write") ) {
    send_msg(id, "command syntax - write 'receiver' 'content'\n");
    return;
  }

  strcpy(temp, msg_buf.message);
  buff = strtok(temp, " ");
  buff = strtok(NULL, " ");
  if( (buff == NULL) ) {
    send_msg(id, "command syntax - leavegroup 'group name'\n");
    return;
  }
  strcat(name, buff);
  buff = strtok(NULL,  "");
  while ( buff != NULL ) {
    strcat(message, buff);
    buff = strtok(NULL, "");
  }

  for (i = 0; i < 9; i++) {
    if ( msg_buf.senderID == users[i].userID ) {
      strcpy(sender, users[i].name);
    }
  }

  currentTime = time(NULL);
  struct tm tm = *localtime(&currentTime);
  sprintf(temp, "%d:%d:%d %s: ", tm.tm_hour, tm.tm_min, tm.tm_sec, sender);
  strcpy(temp1, temp);
  strcat(temp1, message);
  strcpy(message, temp1);

  for(i = 0; i < 9; i++) {
    if ( !strcmp(users[i].name, name) ) {
      if ( users[i].isLogged ) {
        send_msg_from_user(users[i].userID, message);
        send_msg(id, "message has been sent\n");
        return;
      }
      else {
        char temp[50] = "";
        sprintf(temp, "user %s is not online\n", name);
        send_msg(id, temp);
        return;
      }
    }
  }
  char tempp[50] = "";
  sprintf(tempp, "no user named %s found.\n", name);
  send_msg(id, tempp);
  return;
}

void check_and_login(int id) {
  if( is_logged(id) ) {
    send_msg(id, "please logout first. \n");
    return;
  }
  user_login(id);
}

void logout(int id) {
  for (i = 0; i < 9; i++) {
    if ( id == users[i].userID ) {
      if ( users[i].isLogged ) {
        users[i].isLogged = false;
        send_msg(id, "you have succesfully logged out. \n");
        printf("user %s has logged out\n", users[i].name);
        return;
      }
    }
  }
  send_msg(id, "!!unknown error in logout() function!!\n");
}

void display_users(int id) {
  char message[400] = "",temp[400] = "";
  int counter = 0;
  for(i = 0; i < 9; i++) {
    if( users[i].isLogged ) {
      strcat(message, users[i].name);
      strcat(message, "\n");
      counter++;
    }
  }
  strcat(message, "\n");
  strcpy(temp, message);
  sprintf(message, "List of online users (%d): \n", counter);
  strcat(message, temp);
  if( counter )
    send_msg(id, message);
  else
    send_msg(id, "No users online. (sth went wrong?)\n"); //uzytkownik wpisujacy to musi byc online
}

void join_group(int id) {
  char temp[100] = "", *name, buff[100] = "";

  strcpy(temp, msg_buf.message);
  name = strtok(temp, " "); //pierwszy pomijamy (komenda)
  name = strtok(NULL, " "); //nazwa grupy
  if( (name == NULL) ) {
    send_msg(id, "command syntax - joingroup 'group name'\n");
    return;
  }

  for (i = 0; i < 9; i++) {
    if ( users[i].userID == id ) { //znajdujemy nr tego usera
      for (j = 0; j < 3; j++) {
        if ( !strcmp(groups[j].name, name) ) {  //znajdujemy grupe
          groups[j].member[ groups[j].counter ].id = id; //id czlonka o indeksie counter grupy jtej = id
          strcpy( groups[j].member[ groups[j].counter ].name, users[i].name ); //uzytkownik ktory wyslal zapytanie zostaje dopisany
          sprintf(buff, "you are now a member of %s\n" , groups[j].name);
          send_msg(id, buff);
          groups[j].counter++;
          return;
        }
      }
    }
  }
  sprintf(buff, "there is no group named %s\n" , name);
  send_msg(id, buff);
  return;
}

void leavegroup(int id) {
  char temp[100] = "", *name, buff[100] = "";
  bool correctName = false, wasMember = false;

  strcpy(temp, msg_buf.message);
  name = strtok(temp, " "); //pierwszy pomijamy (komenda)
  name = strtok(NULL, " "); //nazwa grupy
  if( (name == NULL) ) {
    send_msg(id, "command syntax - leavegroup 'group name'\n");
    return;
  }

  for (i = 0; i < 3; i++) {
    if ( !strcmp(name, groups[i].name) ) { //znajdujemy szukana grupe
      correctName = true;
      for (j = 0; j < 9; j++) {
        if ( groups[i].member[j].id == id ) { //znajdujemy uzytkownika i zerujemy jego dane w grupie
          groups[i].member[j].id = 0;
          strcpy(groups[i].member[j].name, "");
          groups[i].counter--;
          wasMember = true;
          sprintf(buff, "you have left %s\n" , name);
          send_msg(id, buff);
          return;
        }
      }
    }
  }
  if( !correctName ) {
    sprintf(buff, "there is no group named %s\n" , name);
    send_msg(id, buff);
    return;
  }
  if( !wasMember ) {
    sprintf(buff, "you are not a member of %s\n" , name);
    send_msg(id, buff);
    return;
  }
}

void show_members(int id) {
  char temp[100] = "", *name, buff[100] = "", message[300] = "";
  bool correctName = false;

  strcpy(temp, msg_buf.message);
  name = strtok(temp, " "); //pierwszy pomijamy (komenda)
  name = strtok(NULL, " "); //nazwa grupy
  if( (name == NULL) ) {
    send_msg(id, "command syntax - showmembers 'group name'\n");
    return;
  }

  for (i = 0; i < 3; i++) {
    if ( !strcmp(name, groups[i].name) ) { //znajdujemy szukana grupe
      correctName = true;
      if( groups[i].counter <= 0 ) {      //pustej grupy nie wyswietlamy
        sprintf(buff, "%s is empty\n" , name);
        send_msg(id, buff);
        return;
      }
      for (j = 0; j < groups[i].counter; j++) {  //wyswietlamy jej czlonkow
        strcat(message, groups[i].member[j].name);
        strcat(message, "\n");
      }
      strcat(message, "\n");
      strcpy(temp, message);
      sprintf(message, "members of %s (%d): \n", groups[i].name, groups[i].counter);
      strcat(message, temp);
      send_msg(id, message);
      return;
      }
    }
  if( !correctName ) {
    sprintf(buff, "there is no group named %s\n" , name);
    send_msg(id, buff);
    return;
  }
}

void msg_group(int id) {
  char *buff = NULL, temp[300] = "", temp1[300] = "", name[100] = "", message[400] = "", sender[100] = "";
  time_t currentTime;
  bool sent = false, found = false;

  if( !strcmp(msg_buf.message, "writegroup") ) {
    send_msg(id, "command syntax - writegroup 'groupname' 'content'\n");
    return;
  }
  strcpy(temp, msg_buf.message);
  buff = strtok(temp, " ");
  buff = strtok(NULL, " ");
  if( (buff == NULL) ) {
    send_msg(id, "command syntax - writegroup 'groupname' 'content'\n");
    return;
  }
  strcat(name, buff);
  buff = strtok(NULL,  "");
  while ( buff != NULL ) {
    strcat(message, buff);
    buff = strtok(NULL, "");
  }
  for (i = 0; i < 9; i++) {
    if ( msg_buf.senderID == users[i].userID ) {
      strcpy(sender, users[i].name);
    }
  }

  currentTime = time(NULL);
  struct tm tm = *localtime(&currentTime);

  sprintf(temp, "%d:%d:%d %s in %s: ", tm.tm_hour, tm.tm_min, tm.tm_sec, sender, name);
  strcpy(temp1, temp);
  strcat(temp1, message);
  strcpy(message, temp1);

  for(i = 0; i < 3; i++) {
    if ( !strcmp(groups[i].name, name) ) { //szukamy odpowiedniej grupy
      found = true;
      for (j = 0; j < groups[i].counter; j++) {   //iterujemy po czlonkach znalezionej grupy
        for (k = 0; k < 9; k++) {
          if ( !strcmp(groups[i].member[j].name, users[k].name) ) { //nazwa czlonka = nazwa uzytkownika wysylamy pod jego id i szukamy nastepnego czlonka
            send_msg_from_user(users[k].userID , message);
            sent = true;
            break;
          }
        }
      }
    }
  }
  if ( !found ) {
    char tempp[50] = "";
    sprintf(tempp, "no group named %s found.\n", name);
    send_msg(id, tempp);
    return;
  }
  else if ( sent ) {
    send_msg(id, "message has been sent\n");
    return;
  }
  else {
    char tempp[50] = "";
    sprintf(tempp, "no member of %s is online.\n", name);
    send_msg(id, tempp);
    return;
  }
}

void display_groups(int id) {
  char message[300] = "";
  strcat(message, "List of avaliable groups (3): \n");
  for(i = 0; i < 3; i++) {
    strcat(message, groups[i].name);
    strcat(message, "\n");
  }
  send_msg(id, message);
}

int main(int argc, char *argv[]) {
  bool userLogged = false;
  int localID;
  load_file("config.txt");
  serverID = msgget(222, IPC_CREAT | 0644);
  char command[50] = "", *buff, temp[50] = "";

  while( !exitProgram ) {                    //nie ma komendy na zamkniecie serwera
    msgrcv(serverID, &msg_buf, sizeof(msg_buf) - sizeof(long), 1, 0);
    localID = msg_buf.senderID;
    userLogged = is_logged(localID);
    strcpy(temp, msg_buf.message);
    buff = strtok(temp, " ");
    if( (buff == NULL) ) {      //pusta wiadomosc
      display_help(localID);
      continue;
    }
    strcat(command, buff);

    if( !strcmp(command, "login") )
      check_and_login(localID);
    else if( !strcmp(command, "123connectedxx1")) //zaszyfrowany komunikat
      send_msg(localID, "Welcome! Type help to get command list and syntax.\n");
    else if ( !strcmp(command, "help") )
      display_help(localID);
    else if ( !strcmp(command, "write") && userLogged )
      msg_user(localID);
    else if ( !strcmp(command, "logout") && userLogged )
      logout(localID);
    else if( !strcmp(command, "read") && userLogged )
      send_msg(localID, "");
    else if( !strcmp(command, "online") && userLogged )
      display_users(localID);
    else if( !strcmp(command, "xxx123removeipc321xxx") ) {
      logout(localID);
      msgctl(localID, IPC_RMID, 0);
      printf("user logged out\n");
    }
    else if( !strcmp(command, "joingroup") && userLogged )
      join_group(localID);
    else if( !strcmp(command, "leavegroup") && userLogged )
      leavegroup(localID);
    else if( !strcmp(command, "showmembers") && userLogged )
      show_members(localID);
    else if( !strcmp(command, "writegroup") && userLogged )
      msg_group(localID);
    else if( !strcmp(command, "showgroups") && userLogged )
      display_groups(localID);
    else if( !strcmp(command, "exit") )
      send_msg(localID, "you have succesfully logged out. Program will now exit.\n");
    else
      send_msg(localID, "type 'help' to display help menu\n");

    userLogged = false;
    strcpy(command, "");
  }
  printf("server shutting down...\n");
  send_msg(msg_buf.senderID, "exit");
  msgctl(serverID, IPC_RMID, 0);

  return 0;
}
