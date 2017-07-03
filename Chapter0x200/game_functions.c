#include"game_functions.h"

//This function reads the player data for the current user_id
//from the file.It returns -1 if it is unable to find player
//data for the current user_id.
int get_player_data()
{
	int file_descriptor;
	int current_user_id;
	int read_bytes;
	struct user entry;

	current_user_id=getuid();

	file_descriptor=open(DATAFILE,"O_RDONLY");
	if(file_descriptor == -1)	//Can't open the file,maybe it doesn't exist
	{
		return -1;
	}

	read_bytes=read(file_descriptor,&entry,sizeof(struct user));	//Read the first chunk
	while(entry.user_id ! current_user_id && read_bytes > 0)	//Loop until proper user is found
	{
		read_bytes=read(file_descriptor,&entry,sizeof(struct user));
	}

	close(file_descriptor);			//Close the file
	if(read_bytes < sizeof(struct user))	//This means that the end of file was reached
	{
		return -1;
	}
	else
	{
		player=entry;			//Copy the read entry into the player struct
	}

	return 1;

}

//This function writes the current player data to the file.
//It is used primarily for updating the credits after games.
void update_player_data()
{
	int file_descriptor;
	int i;
	int read_user_id;
	char burned_byte;

	file_descriptor=open(DATAFILE,O_RDWR);
	if(file_descriptor == -1)	//If open fails here,something is really wrong.
	{
		fatal("in update_player_data() while opening file");
	}
	read(file_descriptor,&read_user_id,4);	//Read the user_id from the first struct
	while(read_user_id != player.user_id)	//Loop until correct user_id is found
	{
		for(i=0;i<sizeof(struct user) -4;i++)	//Read through the rest of that struct.
		{
			read(file_descriptor,&burned_byte,1);
		}
		read(file_descriptor,&read_user_id,4);	//Read the user_id from the next struct.
	}
	write(file_descriptor,&(player.credits),4);	//Update credits
	write(file_descriptor,&(player.highscore),4);	//Update highscore
	write(file_descriptor,&(player.name),100);	//Update name
	close(file_descriptor);
}
//This is the new user registration function.
//It will create a new player account and append it to the file.
void register_new_player()
{
	int file_descriptor;
	
	printf("-=-={ New Player Registration }=-=-\n");
	printf("Enter your name: ");
	input_name();

	player.user_id=getuid();
	player.highscore=player.credits=100;

	file_descriptor=open(DATAFILE,O_WRONLY|O_CREAT|O_APPEND,S_IWUSR|S_IRUSR);
	if(file_descriptor == -1)
	{
		fatal("in register_new_player() while opening file.");
	}
	write(file_descriptor,&player,sizeof(struct user));
	close(file_descriptor);

	printf("\nWelcome to the Game of Chance %s\n",player.name);
	printf("You have been given %u credits.\n",player.credits);
}

//This function is used to input the player name,since
//scanf("%s",&whatever) will stop input at the first space.
void input_name()
{
	char *name_ptr;
	char input_char='\n';
	while(input_char == '\n')	//Flush any leftover
	{
		scanf("%c",&input_char);//Newline chars
	}

	name_ptr=(char*)&(player.name);	//name_ptr=player name's address
	while(input_char != '\n')	//Loop until newline
	{
		*name_ptr=input_char;	//Put the input char into name field
		scanf("%c",&input_char);//Get the next char
		name_ptr++;		//Increment the name pointer
	}
	*name_ptr=0;			//Terminate the string
}

//This function will display the current high score and
//the name of the person who set that high score.
void show_highscore()
{
	unsigned int top_score=0;
	char top_name[100];
	struct user entry;
	int file_descriptor;
	
	printf("\n=================================================| HIGH SCORE |======================================\n");
	file_descriptor=open(DATAFILE,O_RDONLY);
	if(file_descriptor == -1)
	{
		fatal("in show_highscore() while opening file");
	}
	while(read(file_descriptor,&entry,sizeof(struct user)) > 0)	//Loop until end of file
	{
		if(entry.highscore > top_score)				//If there is a higher score
		{
			top_score=entry.highscore;			//set top_score to that score
			strcpy(top_name,entry.name);			//and top_name to that username
		}
	}
	close(file_descriptor);
	if(top_score > player.highscore)
	{
		printf("%s has the high score of %u\n",top_name,top_score);
	}
	else
	{
		printf("You currently have the high score of %u credits!\n",player.highscore);
	}
	printf("==========================================================================\n\n");
}

//This function simply awards the jackpot for the Pick a Number game
void jackpot()
{
	printf("*+*+*+*+*+*+*+* JACKPOT *+*+*+*+*+*+*+*+*");
	printf("You have won the jackpot of 100 credits!\n");
	player.credits += 100;
}

//This function prints the 3 cards for the Find the Ace game.
//It expects a message to display,a pointer to the cards array,
//-1,then the selection numbers are displayed.
void print_cards(char *message,char* cards,int user_pick)
{
	int i;

	printf("\n\t*** %s ***\n",message);
	printf("	\t._.\t._.\t._.\n");
	printf("Cards:\t|%c|\t|%c|\t|%c|\n\t",cards[0],cards[1],cards[2]);
	if(user_pick == -1)
	{
		printf(" 1 \t 2 \t 3 \n");
	}
	else
	{
		for(i=0;i < user_pick;i++)
		{
			printf("\t");
		}
		printf(" ^-- your pick\n");
	}
}

//This function inputs wagers for both the No Match Dealer and
//Find the Ace games. It expects the available credits and the
//previous wager as arguments. The previous_wager is only important
//for the second wager in the Find the Ace game.The function
//returns -1 if the wager is too big or too little, and it returns
//the wager amount otherwise.
int take_wager(int available_credits,int previous_wager)
{
	int wager;
	int total_wager;

	printf("How many of your %d credits would you like to wager? ",available_wager);
	scanf("%d",&wager);

	if(wager < 1)	//Make sure the wager is greater than 0.
	{
		printf("Nice try,but you must wager a positive number!\n");
		return -1;
	}
	total_wager=previous_wager+wager;
	if(total_wager > available_wager)	//Confirm available credits
	{
		printf("Your total wager of %d is more than you have!\n",total_wager);
		printf("You only have %d available credits,try again.\n",available_wager);
		return -1;
	}

	return wager;
}

//This function contains a loop to allow the current game to be
//played again.It also writes the new credit totals to file
//after each game is played.
void play_the_game()
{
	int play_again=1;
	int (*game)();
	char selection;

	while(play_again)
	{
		printf("\n[DEBUG] current_game pointer @ 0x%08x\n",player.current_game);
		if(player.current_game() != -1)	//If the game plays without error and a new high score is set
		{
			if(player.credits > player.highscore)
			{
				player.highscore=player.credits;	//Update the highscore
			}
			printf("\nYou now have %u credits\n",player.credits);
			update_player_data();	//Write the new credit total to file.
			printf("Would you like to play again?(y/n) ");
			selection='\n';
			while(selection == '\n')//Flush any extra newlines.
			{
				scanf("%c",&selection);
			}
			if(selection == 'n')
			{
				play_again=0;
			}
		}
		else			//This means the game returned an error,
		{
			play_again=0;	//so return to main menu.
		}
	}
}

//This function is the Pick a Number game
//It returns -1 if the player doesn't have enough credits.
int pick_a_number()
{
	int pick;
	int winning_number;

	printf("\n########################### Pick a Number ####################\n");
	printf("This game costs 10 credits to play.Simply pick a number\n");
	printf("between 1 and 20, and if you pick the winning number,you\n");
	printf("will win the jackpot of 100 credits!\n\n");
	winning_number=(rand() % 20) + 1;	//Pick a number between 1 and 20.
	if(player.credits < 10)

