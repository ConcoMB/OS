#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "list.h"
#include <pthread.h>
#include "join.h"
#include "display.h"
#include "league.h"
#include <sys/shm.h>
#include <signal.h>

client_t* myClient;

void makeConnection();
void start();
void logClient();
void makeDisconnection();

void* clientAtt(void* arg)
{
	printf("entre al attendant\n");
	myClient=(client_t*)arg;
	makeConnection();
	logClient();
	start();
}

void logClient()
{
	int aux, loged=0, msg;
	char name[NAME_LENGTH], password[NAME_LENGTH];
	while(!loged)
	{
		if(!rcvMsg(myClient->readFD,(void*)&msg, sizeof(int)))
		{
			makeDisconnection();
		}
		printf("msg: %d\n", msg);
		if(!rcvString(myClient->readFD, name))
		{
			makeDisconnection();
		}
		printf("name: %s\n", name);
		if(!rcvString(myClient->readFD, password))
		{
			makeDisconnection();
		}
		printf("psswd: %s\n", password);
		if(msg==LOGIN)
		{
			if((aux=logIn(name,password, myClient))==0)
			{
				loged=1;
			}
			sndMsg(myClient->writeFD, (void*)&aux, sizeof(int));
		}
		else if(msg==SIGNUP)
		{
			printf("Hago el signup\n");
			aux=signUp(name, password);
			sndMsg(myClient->writeFD, (void*)&aux, sizeof(int));
			if(aux==0)
			{
			    loged=1;
			    printf("voy al login\n");
			    logIn(name, password, myClient);
			}
		}
	}
}

void start()
{
	int msg;
	while(1)
	{
		rcvMsg(myClient->readFD,(void*)&msg, sizeof(int));
		printf("%d\n",msg);
		switch(msg)
		{
			case SEND_LEAGUE:
				listLeagues(myClient->writeFD);
				break;
			case SEND_TEAM:
				listTeam(myClient->user ,myClient->writeFD);
				break;
			case SEND_TRADE:
				listTrades(myClient->user, myClient->writeFD);
				break; 
			case LEAGUE_SHOW:
				rcvMsg(myClient->readFD, (void*)&msg, sizeof(int));
				if(msg<lCant&&msg>=0)
				{
					leagueShow(leagues[msg], myClient->writeFD);
				}
				else
				{
					msg=LEAGUE_ID_INVALID;
					sndMsg(myClient->writeFD, (void*)&msg, sizeof(int));
				}
				break;
		}
	}
}

void makeDisconnection()
{
	disconnect(myClient->readFD);
	disconnect(myClient->writeFD);
	printf("cliente desconectado\n");
	pthread_exit(0);
}

void makeConnection()
{
	char readChannel[4], writeChannel[4];
	int id=myClient->ID;
	sprintf(writeChannel, "%c%d", 's', id);
	create(writeChannel);
	myClient->writeFD=connect(writeChannel, O_WRONLY);
	sprintf(readChannel, "%c%d", 'c', id);
	create(readChannel);
	myClient->readFD=connect(readChannel, O_RDONLY);
}
