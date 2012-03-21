#include "files.h"
#include "arrays.h"
#include "league.h"
#include "externvars.h"

league_t** leagues;
int lCant, uCant;
user_t** users;

void printAll(void);

int main(void)
{
  loadAll();
  trade_t trade={2,NULL,NULL,NULL};
  insert(leagues[0]->trades,&trade);
  reset(leagues[0]->trades);
  printf("%d\n",((trade_t*)getNext(leagues[0]->trades))->ID);
  printAll();  
  saveAll();
  return 0;
}

void printAll()
{
  int i;
  printf("-- USERS --\n");
  for(i=0;i<uCant;i++)
  {
    printf("%d %s %s\n",users[i]->ID, users[i]->name, users[i]->password);
  }
  printf("-- LEAGUES --\n");
  for(i=0;i<lCant;i++)
  {
    int j;
    printf("%d %s %s\n",leagues[i]->ID, leagues[i]->name, leagues[i]->password);
    printf("	-- TEAMS --\n");
    for(j=0;j<leagues[i]->tCant;j++){
      team_t* team=leagues[i]->teams[j];
      printf("	%d %d %s\n",team->ID, team->user->ID, team->name);
    }
    printf("	-- SPORTIST -- \n");
    for(j=0;j<CANT_SPORTIST&&leagues[i]->sportists[j];j++)
    {
      sportist_t* sportist=leagues[i]->sportists[j];
      printf("	%d %d %s\n",sportist->ID, sportist->team->ID, sportist->name);
    }
    printf("	-- TRADES -- \n");
    trade_t* trade;
    reset(leagues[i]->trades);
    while(trade=(trade_t*)getNext(leagues[i]->trades))
    {
      printf("	%d %d %d %d\n",trade->ID, trade->from->ID, trade->to->ID, trade->offer->ID, trade->change->ID);
    }
  }
}
