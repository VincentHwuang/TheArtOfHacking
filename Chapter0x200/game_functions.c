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
	while(entry.user_id != current_user_id && read_bytes > 0)	//Loop until proper user is found
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
	printf("      \t._.\t._.\t._.\n");
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

	printf("How many of your %d credits would you like to wager? ",available_credits);
	scanf("%d",&wager);

	if(wager < 1)	//Make sure the wager is greater than 0.
	{
		printf("Nice try,but you must wager a positive number!\n");
		return -1;
	}
	total_wager=previous_wager+wager;
	if(total_wager > available_credits)//Confirm available credits
	{
		printf("Your total wager of %d is more than you have!\n",total_wager);
		printf("You only have %d available credits,try again.\n",available_credits);
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
	{
		printf("You only have %d credits. That's not enough to play!\n\n",player.credits);
		return -1;			//Not enough credits to play
	}
	player.credits -= 10;			//Deduct 10 credits
	printf("10 credits have been deducted from your account.\n");
	printf("Pick a number between 1 and 20: ");
	scanf("%d",&pick);

	printf("The winning number is %d\n",winning_number);
	if(pick == winning_number)
	{
		jackpot();
	}
	else
	{
		printf("Sorry,you didn't win.\n");
	}

	return 0;
}

//This is the No Match Dealer game.
//It returns -1 if the player has 0 credits
int dealer_no_match()
{
	int i;
	int j;
	int number[16];
	int wager=-1;
	int match=-1;

	printf("\n::::::::::::::::::: No Match Dealer :::::::::::::::\n");
	printf("In this game,you can wager up to all of your credits.\n");
	printf("The dealer will deal out 16 random numbers between 0 and 99.\n");
	printf("If there are no matches among them,you double your money!\n\n");

	if(player.credits == 0)
	{
		printf("You don't have any credits to wager!\n\n");
		return -1;
	}
	while(wager == -1)
	{
		wager=take_wager(player.credits,0);
	}
	printf("\t\t::: Dealer out 16 random numbers :::\n");
	for(i=0;i<16;i++)
	{
		number[i]=rand()%100;		//Pick a number between 0 and 99
		printf("%2d\t",number[i]);	
		if(i%8 == 7)			//Print a line break every 8 numbers
		{
			printf("\n");
		}
	}
	for(i=0;i<15;i++)			//Loop looking for matches
	{
		j=i+1;
		while(j<16)
		{
			if(number[i] == number[j])
			{
				match=number[i];
			}
			j++;
		}
	}
	if(match != -1)
	{
		printf("The dealer matched the number %d!\n",match);
		printf("You lose %d credits.\n",wager);
		player.credits -= wager;
	}
	else
	{
		printf("There were no matches!You win %d credits!\n",wager);
		player.credits += wager;
	}

	return 0;
}

//This is the Find the Ace game
//It returns -1 if the player has 0 credits.
int find_the_ace()
{
	int i;
	int ace;
	int total_wager;
	int invalid_choice;
	int pick=-1;
	int wager_one=-1;
	int wager_two=-1;
	char choice_two;
	char cards[3]={'X','X','X'};

	ace=rand()%3;	//Place the ace randomly.

	printf("************* Find the Ace *********************\n");
	printf("In this game,you can wager up to all of your credits.\n");
	printf("Three cards will be dealt out,two queens and one ace.\n");
	printf("If you find the ace,you will win your wager.\n");
	printf("After choosing a card,one of the queens will be revealed.\n");
	printf("At this point,you may either select a different card or\n");
	printf("increase your wager.\n\n");

	if(player.credits == 0)
	{
		printf("You don't have any credits to wager!\n\n");
		return -1;
	}

	while(wager_one == -1)		//Loop until valid wager is made.
	{
		wager_one=take_wager(player.credits,0);
	}

	print_cards("Dealing cards",cards,-1);
	pick=-1;
	while((pick < 1) || (pick > 3))	//Loop until valid pick is made.
	{
		printf("Select a card:1,2,or 3 ");
		scanf("%d",&pick);
	}
	pick--;				//Adjust the pick since card numbering starts at 0.
	i=0;
	while(i == ace || i == pick)	//Keep looping until we find a valid queen to reveal
	{
		i++;
	}
	cards[i]='Q';
	print_cards("Revealing a queen",cards,pick);
	invalid_choice=1;
	while(invalid_choice)		//Loop until valid choice is made.
	{
		printf("Would you like to:\n[c]hange your pick\tor\t[i]ncrease your wage?\n");
		printf("Select c or i: ");
		choice_two='\n';
		while(choice_two == '\n') 	//Flush extra newlines
		{
			scanf("%c",&choice_two);
		}
		if(choice_two == 'i')	 	//Increase wager
		{
			invalid_choice=0;	//This is a valid choice.
			while(wager_two == -1)	//Loop until valid second wager is made
			{
				wager_two = take_wager(player.credits,wager_one);
			}
		}
		if(choice_two == 'c')		//Change pick
		{
			i=invalid_choice=0;	//Valid choice.
			while(i == pick || cards[i] == 'Q')	//Loop until the other card is found
			{
				i++;
			}
			pick=i;			//and then swap pick
			printf("Your card pick has been changed to card %d\n",pick+1);
		}
	}

	for(i=0;i<3;i++)			//Reveal all of the cards
	{
		if(ace == i)
		{
			cards[i]='A';
		}
		else
		{
			cards[i]='Q';
		}
	}
	print_cards("End result",cards,pick);

	if(pick == ace)				//Handle win
	{
		printf("You have won %d credits from your first wager\n",wager_one);
		player.credits += wager_one;
		if(wager_two != -1)
		{
			printf("and an additional %d credits from your second wager!\n",wager_two);
			player.credits += wager_two;
		}
	}
	else					//Handle loss
	{
		printf("You have lost %d credits from your first wager\n",wager_one);
		player.credits -= wager_one;
		if(wager_two != -1)
		{
			printf("and an additional %d credits from your second wager!\n",wager_two);
			player.credits -= wager_two;
		}
	}

	return 0;
}
