#ifndef __JOIN__
#define __JOIN__

#include "league.h"
#include "../msg.h"
#include <string.h>
#include "arrays.h"
#include "externvars.h"
#include "files.h"
#include "cmp.h"
#include "list.h"


int userNameOccupied(char* name);

int teamNameOccupied(league_t* league, char name[NAME_LENGTH]);

int signUp(char* name, char* password);

int logIn(char* name, char* password, client_t* client);

int userAlreadyJoined(league_t* league, user_t* user);

int joinLeague(user_t* user, league_t* league, char* teamName, char* password);

int leagueNameOccupied(char* name);

int createLeague(char* name, char* password, int cant);


#endif