#include "../common.h"
#include "../msg.h"
#include <stdio.h>

int main(int argc, char** args)
{
	int msg = LEAGUE_SHOW;
	char readChannel[4], writeChannel[4];
    strcpy(readChannel, args[2]);
    strcpy(writeChannel, args[1]);
    int leagueID=(int)args[3];
    sndMsg(writeChannel, (void*)&msg, sizeof(int));
    sndMsg(writeChannel, (void*)&leagueID, sizeof(int));
    rcvMsg(readChannel, (void*)&msg, sizeof(int));
    if(msg==LEAGUE_ID_INVALID)
    {
	 printf("invalid league id\n");
	 exit(0);
    }
    while(msg==LEAGUE_SHOW&&msg!=END_LEAGUE_SHOW)
    {
      char string[50];
      rcvString(readChannel, string);
      printf("%s", string);
          rcvMsg(readChannel, (void*)&msg, sizeof(int));

    }
    
      exit(0);
}