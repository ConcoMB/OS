#include "connection.h"
#include "../common.h"
#include "../msg.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** args)
{
  printf("entre\n");
  int msg, clientID;
  void* channel;
  clientID=atoi(args[1]);
  connectClient(clientID,&channel);
  int ID=atoi(args[2]);
  msg=TRADE_SHOW;
  char string[50];
  sndMsg(channel, (void*)&msg, sizeof(int));
  sndMsg(channel, (void*)&ID, sizeof(int));
  rcvMsg(channel, (void*)&msg, sizeof(int));
  if(msg==ID_INVALID)
  {
  	printf("ID INVALID\n");
  }
  else
  {
  	rcvString(channel, string);
  	printf("%s\n", string);
  }
  exit(0);
}
