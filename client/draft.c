#include "../common.h"
#include "../msg.h"
#include <stdio.h>
#include <pthread.h>
#include "connection.h"
#include <time.h>
#include <stdlib.h>

int flag;
int clientID;

void * quitThread(void* channel);
void* spChooser(void* channel);

int main(int argc, char** args)
{
	pthread_t quitT;
  time_t start, now;
  double diff=0, end;
  void* channel, *quitChannel;
  clientID=atoi(args[1]);
  connectClient(clientID,&channel);
  quitChannel=connectChannel(DEFAULTID+1);
  int ID=atoi(args[2]);
  int msg=DRAFT;
  printf("Entre a draft fork\n");
  sndMsg(channel, (void*)&msg, sizeof(int));
  sndMsg(channel, (void*)&ID, sizeof(int));
  rcvMsg(channel, (void*)&msg, sizeof(int));
  printf("Recibi mensajes\n");
  pthread_create(&quitT, NULL, quitThread, quitChannel);
  if(msg==ID_INVALID)
  {
  	printf("INVALID ID\n");
  	exit(0);
  }
  if(msg==DRAFT_WAIT)
  {
  	printf("Waiting for other teams...\n");
  	rcvMsg(channel, (void*)&msg, sizeof(int));
  	if(msg==DRAFT_BEGUN)
  	{
  		printf("Draft has begun\n");
	  	rcvMsg(channel, (void*)&msg, sizeof(int));
	  	while(msg!=END_DRAFT)
	  	{
	  		if(msg==YOUR_TURN)
	  		{
	  			diff=0;
	  			printf("Its your turn to pick!!\n");
	  			sleep(1);
	  			int i;
	  			char string[200];
	  			pthread_cancel(quitT);
	  			for(i=0; i<CANT_SPORTIST; i++)
	  			{
		 			rcvMsg(channel, (void*)&msg, sizeof(int));
		 			rcvString(channel, string);
		 			printf("sportist: %s", string);
	  			}
	  			flag=0;
	  			pthread_t sportThrd;
	  		  	rcvMsg(channel,(void*)&end, sizeof(double));
	  			start=time(NULL);
	  			//printf("%f tiempo \n", end);
	  			pthread_create(&sportThrd, NULL, spChooser, channel);
	  			while(diff<=end && !flag)
				{
					now=time(NULL);
					diff=difftime(now, start);
				}
				//pthread_join(sportThrd, NULL);
				pthread_cancel(sportThrd);
				if(!flag) //NO SE ELIGIO
				{	
		 			rcvMsg(channel, (void*)&msg, sizeof(int));
					printf("Time ellapsed, you have a random sportist, ID %d\n",msg);
				}
				pthread_create(&quitT, NULL, quitThread, quitChannel);

	  		}
	  		else if(msg==DRAFT_WAIT)
	  		{
	  			printf("The other players are picking teams, please wait.\n");
	  		}
	  		rcvMsg(channel, (void*)&msg, sizeof(int));
	  	}
	  	printf("Draft ended\n");
	  }
  }
  exit(0);
}

void* spChooser(void* channel)
{
	int msg;
	while(1)
	{
		printf("Please choose your sportist: type its ID\n");	
		scanf("%d", &msg);
		printf("lei %d\n", msg);
		fflush(stdout);
		sndMsg(channel, (void*)&msg, sizeof(int));
		rcvMsg(channel, (void*)&msg, sizeof(int));
		if(msg==ID_INVALID)
		{
			printf("Invalid sportists ID\n");
		}
		else if(msg==DRAFT_OK)
		{
			printf("You now have your desired sportist\n");
			flag=1;
			pthread_exit(0);
		}
	}
	return NULL;
}

void * quitThread(void* channel)
{
	char  string[10];
	int msg;
	while(1)
	{
		scanf("%s", string);
		if(strcmp(string,"quit")==0)
		{
			printf("exiting draft\n");
			msg=QUIT_DRAFT+clientID;
			printf("%d\n", clientID);
			sndMsg(channel, (void*)&msg, sizeof(int));
			exit(0);
		}
	}
	return NULL;
}
