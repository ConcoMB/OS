#include "files.h"

/* SAVE */

void saveAll(){
    saveUsers();
    saveLeagues();
}

void saveUsers(){
    FILE* userFile;
    user_t* user;
    userFile=fopen("./users.txt","w");
    int uid;
    for(uid=0; uid<uCant;uid++)
    {
		user=users[uid];
		saveUser(userFile, user);
    }
    fclose(userFile);
}

static void saveUser(FILE* userFile, user_t* user){	
	fprintf(userFile, "%d %s %s\n", user->ID, user->name, user->password);
}

void saveLeagues(){
    FILE* leagueFile, *teamFile, *sportistFile, *tradeFile;
    league_t* league;
    team_t* team;
    trade_t* trade;
    sportist_t* sportist;
    leagueFile=fopen("./leagues.txt","w");
    teamFile=fopen("./teams.txt","w");
    sportistFile=fopen("./sportist.txt","w");
    tradeFile=fopen("./trades.txt","w");
    int lid;
    for(lid=0;lid<lCant;lid++)
    {
		league=leagues[lid];
		saveLeague(leagueFile,league);
		char filename[20];
		sprintf(filename,"%d_scores.txt",league->ID);
		sportistFile=fopen(filename,"w");
		reset(league->trades);
		while(trade=(trade_t*)getNext(league->trades)){
			saveTrade(tradeFile, trade);
		}
		int tid;
		for(tid=0;tid<league->tCant;tid++)
		{
			team=league->teams[tid];
			saveTrade(tradeFile, trade);
		}
		int sid;
		for(sid=0;sid<CANT_SPORTIST;sid++)
		{
			sportist=league->sportists[sid];
			saveSportist(sportistFile, sportist);
		}
    }
    fclose(leagueFile);
    fclose(teamFile);
    fclose(sportistFile);
    fclose(tradeFile);
}

static void saveLeague(FILE* leagueFile, league_t* league){
	fprintf(leagueFile, "%d %s %s\n", league->ID, league->password, league->name);
}

void saveTeam(FILE* teamFile, team_t* team){
	fprintf(teamFile, "%d %s %d %d %d\n", team->ID, team->name, team->user->ID, team->league->ID ,team->points);
}

void saveTrade(FILE* tradeFile, trade_t* trade){
	fprintf(tradeFile, "%d %d %d %d %d %d\n", trade->ID, trade->league->ID, trade->from->ID, trade->to->ID, trade->offer->ID, trade->change->ID);
}

void saveSportist(FILE* sportistFile, sportist_t* sportist){
	fprintf(sportistFile, "%d %d %d", sportist->ID, sportist->score, sportist->team->ID);
}

/* LOAD */

void loadAll(){

	loadUsers();
	loadLeagues();
}

void loadUsers(){
	FILE* userFile;
	user_t* user;
	userFile=fopen("./users.txt","r");
	while(user=loadUser(userFile)){
		 newUser(user);
	}
	fclose(userFile);	
}

static user_t* loadUser(FILE* userFile){
	user_t* user;
	user=malloc(sizeof(user_t));
	if(fscanf(userFile, "%d %s %s\n", &user->ID, user->name, user->password)!=EOF){
		return user;
	}
	free(user);
	return NULL;
}

void loadLeagues(){
	FILE* leagueFile;
	league_t* league;
	leagueFile=fopen("./leagues.txt","r");
	while(league=loadLeague(leagueFile)){
		newLeague(league);
	}
	fclose(leagueFile);
}

static league_t* loadLeague(FILE* leagueFile){
	league_t* league;
	league=malloc(sizeof(league_t));
	if(fscanf(leagueFile, "%d %s %s\n", &league->ID, league->password, league->name)!=EOF){
		loadTeams(league);
		loadTrades(league);
		loadSportists(league);
		return league;
	}
	free(league);
	return NULL;
}

/*void loadNewSportists(listADT sportists){
	FILE* sportistFile;
	sportist_t* sportist;
	int sportistID;
	
	sportistFile=fopen("newSportists.txt","r");
	while(scanf(sportistFile,"%d %s",&sportistID)){
		sportist=malloc(sizeof(sportist_t));
		sportist->ID=sportistID;
		fgets(sportist->name, 30, sportistFile);
	}
	
	fclose(sportistFile);
}*/

void loadTeams(league_t* league){
	FILE* teamFile;
	team_t* team;
	char filename[11];
	sprintf(filename,"%d_teams.txt", league->ID);
	teamFile=fopen(filename, "r");
	while(team=loadTeam(teamFile, league)){
		newTeam(league, team);
	}
	fclose(teamFile);
}

static team_t* loadTeam(FILE* teamFile, league_t* league){

	int userID, leagueID, sportistID;
	team_t* team;
	team=malloc(sizeof(team_t));
	
	/* Read data from file */
	if(fscanf(teamFile, "%d %s %d %d", &team->ID, team->name, &userID, &team->points)!=EOF){
		/* Link user */
		team->user=users[userID];
		insert(team->user->teams,team);
		
		/* Link league */
		team->league=league;
		return team;
	}
	free(team);
	return NULL;	
}

void loadTrades(league_t* league){
	FILE* tradeFile;
	trade_t* trade;
	char filename[11];
	sprintf(filename,"%d_trades.txt", league->ID);
	tradeFile=fopen(filename, "r");
	while(trade=loadTrade(tradeFile, league)){
		insert(league->trades, trade);
	}
	fclose(tradeFile);
}

static trade_t* loadTrade(FILE* tradeFile, league_t* league){
	
	int fromID, toID, offerID, changeID, leagueID;
	trade_t* trade;
	trade=malloc(sizeof(trade_t));
	
	/* Read data from file */
	
	if(fscanf(tradeFile, "%d %d %d %d %d", &trade->ID, &fromID, &toID, &offerID, &changeID)!=EOF){
		/* Link Users */
		trade->from=league->teams[fromID];
		trade->to=league->teams[toID];
		
		/* Link League */
		trade->league=league;
		
		/* Link Sportists */
		trade->offer=league->sportists[offerID];
		trade->change=league->sportists[changeID];
		
		return trade;
	}
	free(trade);
	return NULL;
}

void loadSportists(league_t* league){
	FILE* sportistFile; 
	sportist_t* sportist;
	char filename[11];
	sprintf(filename,"%d_scores.txt",league->ID);
	sportistFile=fopen(filename,"r");
	while(sportist=loadSportist(sportistFile, league)){
		newSportist(league, sportist);
	}
	fclose(sportistFile);
}

sportist_t* loadSportist(FILE* sportistFile, league_t* league){
	int sportistID, score, teamID;
	sportist_t* sportist;
	sportist=malloc(sizeof(sportist_t));
	if(fscanf(sportistFile,"%d %d %d %s", &sportist->ID, &sportist->score, &teamID, sportist->name)){
		/* Link Team */
		sportist->team=league->teams[teamID];
		return sportist;
	}
	free(sportist);
	return NULL;
}
