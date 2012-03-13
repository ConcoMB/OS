#ifndef __LEAGUE__
#define __LEAGUE__
#include "list.h"

typedef struct
{
    int ID;
    listADT teams;
    char name[10];
    char password[10];
}user_t;
    

typedef struct
{
	int ID;
    int nextTeamID;
    char name[15];
    listADT sportists;
    listADT teams;
    listADT trades;
    char password[15];
}league_t;


typedef struct
{
	int ID;
    char name[20];
    user_t* user; 
    league_t* league;
    int points;
    listADT sportists;
}team_t;
    
typedef  struct
{
	int ID;
	char name[30];
	int score;
	team_t* team;
}sportist_t;
    
typedef struct 
{
	int ID;
	league_t* league;
    team_t* from;
    team_t* to;
    sportist_t* offer;
    sportist_t* change;
}trade_t;

#endif
