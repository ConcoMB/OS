#include "commands.h"
#include "draft.c"


void (*cmds[])(client_t*)=
{
	cmdSendLeague,
	cmdSendTeam,
	cmdSendTrade,
	cmdLeagueShow,
	cmdTeamShow,
	cmdTradeShow,
	cmdDraft,
	cmdMakeTrade,
	cmdTradeWD,
	cmdTradeYes,
	cmdTradeNeg,
	cmdMakeLeague,
	cmdJoinLeague,
};

int controlDraft(draft_t* draft)
{
	int i;
	for(i=0; i< (draft->league->tMax) ;i++)
	{
		if(draft->clients[i]==NULL)
		{
			return 0;
		}
	}
	return 1;
}


void cmdSendLeague(client_t* myClient)
{
	listLeagues(myClient->channel);
}

void cmdSendTeam(client_t* myClient)
{
	listTeam(myClient->user ,myClient->channel);
}

void cmdSendTrade(client_t* myClient)
{
	listTrades(myClient->user, myClient->channel);
}

void cmdLeagueShow(client_t* myClient)
{
	int msg;
	rcvMsg(myClient->channel, (void*)&msg, sizeof(int));
	if(msg<lCant && msg>=0)
	{
		leagueShow(leagues[msg], myClient->channel, LEAGUE_SHOW, END_LEAGUE_SHOW);
	}
	else
	{
		msg=ID_INVALID;
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
}

void cmdTeamShow(client_t* myClient)
{
	int msg, lID, tID;
	rcvMsg(myClient->channel, (void*)&msg, sizeof(int));
	lID=msg/CONVERSION;
	tID=msg%CONVERSION;
	if(lID<lCant && lID>=0)
	{
		if(tID<leagues[lID]->tCant && tID>=0)
		{
			teamShow(leagues[lID]->teams[tID], myClient->channel, TEAM_SHOW, END_TEAM_SHOW);
		}
		else
		{
			msg=ID_INVALID;
			sndMsg(myClient->channel, (void*)&msg, sizeof(int));
		}
	}
	else
	{
		msg=ID_INVALID;
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
}

void cmdTradeShow(client_t* myClient)
{
	trade_t* trade;
	int msg, lID, tID;
	rcvMsg(myClient->channel, (void*)&msg, sizeof(int));
	lID=msg/CONVERSION;
	tID=msg%CONVERSION;
 	if(lID<lCant && lID>=0)
	{
		trade= getTradeByID(leagues[lID], tID);
		if(trade!=NULL)
		{
			tradeShow(trade, myClient->channel);
		}
		else
		{
			msg=ID_INVALID;
			sndMsg(myClient->channel, (void*)&msg, sizeof(int));
		}
	}
	else
	{
		msg=ID_INVALID;
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
}

void cmdDraft(client_t* myClient)
{
	int msg, auxID;
	rcvMsg(myClient->channel, (void*)&auxID, sizeof(int));
	if(auxID>=0 && auxID<lCant && leagues[auxID]->draft!=NULL)
	{
		team_t* team= getTeamByClient(leagues[auxID], myClient);
		if(team==NULL)
		{
			msg=ID_INVALID;
			sndMsg(myClient->channel, (void*)&msg, sizeof(int));
		}
		else
		{
			if(team->user->draftLeague==-1 || leagues[auxID]->draft->flag == -1)
			{
				//no quiteo
				team->user->draftLeague=leagues[auxID]->ID;
				leagues[auxID]->draft->clients[team->ID]=myClient;
				char semName[20];
				sprintf(semName,"/semDraft%d_Cli%d",leagues[auxID]->ID, myClient->ID);
				leagues[auxID]->draft->sem[team->ID]=sem_open(semName, O_RDWR|O_CREAT, 0666, 0);
				msg=DRAFT_WAIT;
				sndMsg(myClient->channel, (void*)&msg, sizeof(int));
				queueStr(printQueue,BLUE"User %s joined draft\n"WHITE,myClient->user->name);
				if(controlDraft(leagues[auxID]->draft))
				{
					pthread_t draftThr;
					pthread_create(&draftThr, NULL, draftAttendant, (void*)(leagues[auxID]->draft));
					//pthread_join(draftThr, NULL);
				}
				sem_wait(leagues[auxID]->draft->sem[team->ID]);
				/*while(leagues[auxID]->draft->clients[team->ID]!=NULL)
				{
					//esperar
				}*/
				sem_unlink(semName);
			}
			else 	//YA ESTABA DRAFTEANDO Y SALIO
			{
				int dl= team->user->draftLeague;
				if(dl<0 || dl>lCant || leagues[dl]->draft==NULL || leagues[dl]->ID!=dl)
				{
					msg=ID_INVALID;
					sndMsg(myClient->channel, (void*)&msg, sizeof(int));
				}
				putIntoDraft(myClient);
			}
			
		}
	}
	else
	{
		msg=ID_INVALID;
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
}

void putIntoDraft(client_t* myClient)
{
	int msg;
	msg=DRAFT_WAIT;
	sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	msg=DRAFT_BEGUN;
	sndMsg(myClient->channel,(void*)&msg, sizeof(int));

	team_t* team= getTeamByClient(leagues[myClient->user->draftLeague], myClient);

	draft_t* myDraft=leagues[myClient->user->draftLeague]->draft;

	if(myDraft->turn==team->ID && myDraft->sent==0)
	{		
		//ES TU TURNO
		msg=YOUR_TURN;
		sndMsg(myClient->channel,(void*)&msg, sizeof(int));
		sendAllSportists(myDraft->league,  myClient->channel, SEND_SPORTIST);
		double remain=DRAFT_TIME - myDraft->diff;
		sndMsg(myClient->channel,(void*)&remain, sizeof(double));
	}
	//LO REINSERTO AL VECTOR PARA QUE SIGA DRAFT
	myDraft->clients[team->ID]=myClient;
	char semName[20];
	sprintf(semName,"/semDraft%d_Cli%d",myDraft->league->ID, myClient->ID);
	myDraft->sem[team->ID]=sem_open(semName, O_RDWR|O_CREAT, 0666, 0);
	queueStr(printQueue,BLUE"User %s rejoined draft\n"WHITE,myClient->user->name);
	if(myDraft->turn==team->ID)
	{
		sem_post(myDraft->chooseSem);
	}
	sem_wait(myDraft->sem[team->ID]);
	/*while(myDraft->clients[team->ID]!=NULL)
	{
		//mientras siga el draft espera;
	}*/
}

void cmdMakeTrade(client_t* myClient)
{
	int lID, offer, change, tID, msg; 
	team_t* team;
	rcvMsg(myClient->channel, (void*)&lID, sizeof(int));
	rcvMsg(myClient->channel, (void*)&offer, sizeof(int));
	rcvMsg(myClient->channel, (void*)&change, sizeof(int));
	if(lID!=-1)
	{
		tID=lID%CONVERSION;
		lID/=CONVERSION;
	}
	else
	{
		lID=offer/CONVERSION;
		tID=-1;
	}
	if(lID==offer/CONVERSION && lID==change/CONVERSION && lID<lCant && lID>=0 &&
		(tID==-1 || (tID<leagues[lID]->tCant && tID>=0)) &&
		(team=getTeamByClient(leagues[lID], myClient))!=NULL && (offer%=CONVERSION)>=0 && (change%=CONVERSION)>=0 &&
		offer<CANT_SPORTIST && change<CANT_SPORTIST && (tID==-1 ||
		team->ID!=leagues[lID]->teams[tID]->ID))
	{
		if(tID!=-1)
		{
			if(offerTrade(leagues[lID], team, leagues[lID]->teams[tID], leagues[lID]->sportists[offer], leagues[lID]->sportists[change])==0)
			{
				msg=TRADE_OFFERED;
			}
			else
			{
				msg=ERROR;
			}
			sndMsg(myClient->channel, (void*)&msg, sizeof(int));
		}
		else
		{
			if(makeTrade(leagues[lID], team, leagues[lID]->sportists[offer], leagues[lID]->sportists[change])==0)
			{
				msg=TRADE_MADE;
			}
			else
			{
				msg=ERROR;
			}
			sndMsg(myClient->channel, (void*)&msg, sizeof(int));
		}
	}
	else
	{
		msg=ID_INVALID;
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
}

void cmdTradeWD(client_t* myClient)
{
	int msg, lID, tID;
	team_t* team;
	rcvMsg(myClient->channel, (void*)&tID, sizeof(int));
	lID=tID/CONVERSION;
	tID%=CONVERSION;
	if(lID<lCant && lID>=0 && (team=getTeamByClient(leagues[lID], myClient))!=NULL)
	{
		if(withdrawTrade(team, tID, leagues[lID])==0)
		{
			msg=TRADE_WD;
		}
		else
		{
			msg=ERROR;
		}
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
	else
	{
		msg= ID_INVALID;
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
}

void cmdTradeYes(client_t* myClient)
{
	int msg, tID, lID;
	trade_t* trade;
	rcvMsg(myClient->channel, (void*)&tID, sizeof(int));
	lID=tID/CONVERSION;
	tID%=CONVERSION;
	if(lID<lCant && lID>=0 && (trade=getTradeByID(leagues[lID], tID))!=NULL)
	{
		acceptTrade(trade, leagues[lID]);
		msg=TRADE_YES;
	}
	else
	{
		msg= ID_INVALID;
	}
	sndMsg(myClient->channel, (void*)&msg, sizeof(int));
}

void cmdTradeNeg(client_t* myClient)
{
	int tID, lID, offer, change, msg;
	trade_t* trade;
	rcvMsg(myClient->channel, (void*)&tID, sizeof(int));
	rcvMsg(myClient->channel, (void*)&offer, sizeof(int));
	rcvMsg(myClient->channel, (void*)&change, sizeof(int));
	lID=tID/CONVERSION;
	tID%=CONVERSION;
	trade=getTradeByID(leagues[lID], tID);
	if(lID==offer/CONVERSION && lID== change/CONVERSION &&
		lID<lCant && lID>=0 && trade!=NULL && offer>=0 &&
		change>=0 && offer<CANT_SPORTIST && change<CANT_SPORTIST &&
		trade->to->user->ID==myClient->user->ID)
	{
		offer%=CONVERSION;
		change&=CONVERSION;
		if(negociate(trade, leagues[lID]->sportists[offer], leagues[lID]->sportists[change], leagues[lID])==0)
		{
			msg=TRADE_NEG;
		}
		else
		{
			msg=ERROR;
		}
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
		}
	else
	{
		msg= ID_INVALID;
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
}

void cmdMakeLeague(client_t* myClient)
{
	int msg;
	char name[NAME_LENGTH], password[NAME_LENGTH];
	rcvString(myClient->channel, name);
	rcvString(myClient->channel, password);
	rcvMsg(myClient->channel, (void*)&msg, sizeof(int));
	msg=createLeague(name, password, msg);
	if(msg==0)
	{
		msg=MAKE_LEAGUE;
	}
	sndMsg(myClient->channel, (void*)&msg, sizeof(int));
}

void cmdJoinLeague(client_t* myClient)
{
	char name[NAME_LENGTH], password[NAME_LENGTH];
	int msg, lID;
	rcvMsg(myClient->channel, (void*)&lID, sizeof(int));
	if(lID<lCant && lID>=0)
	{
		if(userAlreadyJoined(leagues[lID], myClient->user))
		{
			msg=USER_ALREADY_JOINED;
			sndMsg(myClient->channel, (void*)&msg, sizeof(int));
		}
		else
		{
			msg=JOIN_LEAGUE;
			sndMsg(myClient->channel, (void*)&msg, sizeof(int));
			rcvString(myClient->channel, name);
			if(teamNameOccupied(leagues[lID], name))
			{
				msg=NAME_TAKEN;
				sndMsg(myClient->channel, (void*)&msg, sizeof(int));
			}
			else
			{
				msg=JOIN_LEAGUE;
				sndMsg(myClient->channel, (void*)&msg, sizeof(int));
				if(leagues[lID]->password[0] == '\0')
				{
					msg=NO_PASSWORD;
					sndMsg(myClient->channel, (void*)&msg, sizeof(int));
					password[0]='\0';
				}
				else
				{
					msg=SEND_PASSWORD;
					sndMsg(myClient->channel, (void*)&msg, sizeof(int));
					rcvString(myClient->channel, password);
				}
				msg = joinLeague(myClient->user, leagues[lID], name, password);
				if(msg==0)
				{
					msg=JOIN_LEAGUE;
				}
				sndMsg(myClient->channel, (void*)&msg, sizeof(int));
			}
		}
	}
	else
	{
		msg=ID_INVALID;
		sndMsg(myClient->channel, (void*)&msg, sizeof(int));
	}
}
