#ifndef __EXT__
#define __EXT__

#include "league.h"
#include "strQueue.h"

//NO SE DONDE PONER ESTAS VARIABLES
extern league_t** leagues;
extern user_t** users;
extern int lCant, uCant;
extern listADT clients;
extern int nextUserID;
extern int nextLeagueID;
extern int nextClientID;
extern strQueue_t printQueue;
extern sem_t* saveSem;

#endif
