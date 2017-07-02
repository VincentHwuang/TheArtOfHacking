#ifndef _GAME_FUNCTIONS
#define _GAME_FUNCTIONS

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>
#include<string.h>
#include"hacking.h"

#define DATAFILE "/var/chance.data"	//File to store user data

//Custom user struct to store information about users
struct user
{
	int user_id;
	int credits;
	int highscore;
	char name[100];
	int (*current_game)();
};

//Global variable
struct user player;	//Player struct

//Function prototypes 
int get_player_data();
void register_new_player();
void input_name();
void update_player_data();
void show_highscore();



#endif
