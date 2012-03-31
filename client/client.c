#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "../msg.h"
#include "../common.h"
#include "connection.h"
#include "shell.h"

void sighandler(int sig);
void userLog();
void start();
void makeDefConnection();
void* channel;
void* defChannel;
int msgID;
void* keepAlive(void* arg);

int main()
{
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT, &sighandler);
	makeDefConnection(&msgID);
	connectClient(msgID,&channel);
	pthread_t keepAliveThread;
	pthread_create(&keepAliveThread, NULL, keepAlive, NULL);
	while(1)
	{
		userLog();
	}
}


void makeDefConnection()
{
	int aux= NEWCLIENT;
	defChannel=connectChannel(DEFAULTID+1);
	sndMsg(defChannel, (void*)&aux, sizeof(int));
	printf("mande\n");
	rcvMsg(defChannel, (void*)&msgID, sizeof(int));
	printf("recibi msgid %d\n", msgID);
}

void userLog()
{
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
			sndMsg(channel, (void*)&aux, sizeof(int));
			printf("name:\n");
			scanf("%s", name);
			sndString(channel, name);
			printf("password:\n");
			scanf("%s", password);
			sndString(channel,password);
			rcvMsg(channel, (void*)&handshake, sizeof(int));
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
			sndMsg(channel, (void*)&aux, sizeof(int));
			printf("Enter new name:\n");
			scanf("%s", name);
			sndString(channel, name);
			printf("password:\n");
			scanf("%s", password);
			sndString(channel, password);
			rcvMsg(channel, (void*)&handshake, sizeof(int));
			switch(handshake)
			{
				case NAME_OCCUPIED:
					printf("User name already taken, choose an other\n");
					break;
				case NAME_OR_PASSWORD_TOO_LARGE:
					printf("The length of the user name and the password must be lower than 15 characters\n");
					break;
				default:
					loged=1;
			}
		}
		else
		{
			printf("invalid command\n");
		}
	}
	shell(msgID);
}

void sighandler(int sig)
{
	disconnect(defChannel);
    disconnect(channel);
    exit(0);
}

void* keepAlive(void* arg)
{
	while(1)
	{
		int msg=CLIENT_ALIVE+msgID;
		sndMsg(defChannel,&msg,sizeof(int));
		sleep(5);
	}
}
