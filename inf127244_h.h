#ifndef serwer_h
#define serwer_h
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>
#include <time.h>

typedef struct msgbuf{
	long mtype;
  int senderID;
  char message[400];
} msgbuf;

typedef struct user {
  char name[100];
  char password[100];
  int userID;
  bool isLogged;
} user;

typedef struct groupMember {
	char name[100];
	int id;
} groupMember;

typedef struct group {
  char name[100];
	int counter;
  groupMember member[9];
} group;

extern msgbuf msg_buf;
extern group groups[5];
extern user users[9];


#endif //serwer_h
