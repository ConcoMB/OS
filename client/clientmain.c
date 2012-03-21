#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../msg.h"
#include "../common.h"
#define QUIT 12345


void userLog(int msgID);
void start();
void makeDefConnection(int * msgID);
char writeChannel[4], readChannel[4];

int main()
{
	int msgID;
	makeDefConnection(&msgID);
	while(1){
		userLog(msgID);
	}
}

void makeDefConnection(int * msgID)
{
	int aux= NEWCLIENT;
	char defWChannel[3], defRChannel[3];
	sprintf(defWChannel, "%c%d", 'c', DEFAULTID);
	sprintf(defRChannel, "%c%d", 's', DEFAULTID);
	connect(defWChannel);
	connect(defRChannel);

	sndMsg(defWChannel, (void*)&aux, sizeof(int));
	printf("mande\n");
	rcvMsg(defRChannel, (void*)msgID, sizeof(int));
	printf("recibi msgid %d\n", *msgID);
}

void userLog(int msgID)
{
	sprintf(readChannel, "%c%d", 's', msgID);
	sprintf(writeChannel, "%c%d", 'c', msgID);
	connect(readChannel);
	connect(writeChannel);
	int loged=0;
	while(!loged)
	{
		int handshake;
		char command[10], name[NAME_LENGTH], password[NAME_LENGTH];
		printf("Type login or signup\n");
		scanf("%s", command);
		if(strcmp(command, "login")==0)
		{
			int aux=LOGIN;
			sndMsg(writeChannel, (void*)&aux, sizeof(int));
			printf("name:\n");
			scanf("%s", name);
			sndString(writeChannel, name);
			printf("password:\n");
			scanf("%s", password);
			sndString(writeChannel,password);
			rcvMsg(readChannel, (void*)&handshake, sizeof(int));
			switch(handshake)
			{
				case INCORRECT_PASSWORD:
					printf("incorrect password\n");
					break;
				case USER_NOT_FOUND:
					printf("user unknown\n");
					break;
				default:
					loged=1;
			}
		}
		else if(strcmp(command, "signup")==0)
		{
			int aux=SIGNUP;
			sndMsg(writeChannel, (void*)&aux, sizeof(int));
			printf("Enter new name:\n");
			scanf("%s", name);
			sndString(writeChannel, name);			
			printf("password:\n");
			scanf("%s", password);
			sndString(writeChannel, password);
			printf("recibiendo handshake\n");
			rcvMsg(readChannel, (void*)&handshake, sizeof(int));
			switch(handshake)
			{
				case NAME_OCCUPIED:
					printf("User name already taken, choose an other\n");
					break;
				case NAME_OR_PASSWORD_TOO_LARGE:
					printf("The length of the user name and the password must be lower than 15 characters\n");
					break;
				default:
					printf("te incribiste piibeee\n");
					loged=1;
			}
		}
		else
		{
			printf("invalid command\n");
		}
	}
	start();
}

void start()
{
	int command;
	char string[20];
	do
	{
		printf("type your command \n");
		scanf("%s", string);
		if(strcmp(string, "quit")==0)
		{
			command==QUIT;
		}
		else if(strcmp(string, "listleagues")==0)
		{
			if(fork())
			{
				wait((int*)0);
			}
			else
			{
				printf("me forkeo\n");
				execl("./listleagues", "listleagues", writeChannel, readChannel, NULL);
			}
		}
		else if(strcmp(string, "listteams")==0)
		{
			if(fork())
			{
				wait((int*)0);
			}
			else
			{
				execl("./listteams", "listteams", writeChannel, readChannel, NULL);
			}
		}/*
		else if(strcmp(string, "listtrades")==0)
		{
			if(fork())
			{
				execl();
			}
			else
			{
				wait((int*)0);
			}
		}
		else if(strcmp(string, "leagueshow")==0)
		{
			if(fork())
			{
				execl();
			}
			else
			{
				wait((int*)0);
			}
		}
		else if(strcmp(string, "teamshow")==0)
		{
			if(fork())
			{
				execl();
			}
			else
			{
				wait((int*)0);
			}
		}
		else if(strcmp(string, "tradeshow")==0)
		{
			if(fork())
			{
				execl();
			}
			else
			{
				wait((int*)0);
			}
		}
		else if(strcmp(string, "trade")==0)
		{
			if(fork())
			{
				execl();
			}
			else
			{
				wait((int*)0);
			}
		}
		else if(strcmp(string, "tradewithdraw")==0)
		{
			if(fork())
			{
				execl();
			}
			else
			{
				wait((int*)0);
			}			
		}
		else if(strcmp(string, "tradeaccept")==0)
		{
			if(fork())
			{
				execl();
			}
			else
			{
				wait((int*)0);
			}			
		}
		else if(strcmp(string, "tradenegociate")==0)
		{
			if(fork())
			{
				execl();
			}
			else
			{
				wait((int*)0);
			}			
		}*/
		else 
		{
			printf("invalid command, type 'help' for help\n");
		}
	}
	while(command!=QUIT);
}