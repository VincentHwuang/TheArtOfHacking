#include"game_functions.h"

int main(void)
{
	int choice;
	int last_game;

	srand(time(0));			//Seed the randomizer with the current time

	if(get_player_data() == -1)	//Try to read player data from file
	{
		register_new_player();	//If there is no data,register a new player
	}

	while(choice != 7)
	{
		printf("-=[ Game of Chance Menu ]=-\n");
		printf("1 - Play the Pick a Number game\n");
		printf("2 - Play the No Match Dealer game\n");
		printf("3 - Play the Find the Ace game\n");
		printf("4 - View current high score\n");
		printf("5 - Change your user name\n");
		printf("6 - Reset your account at 100 credits\n");
		printf("7 - Quit\n");
		printf("[Name: %s]\n",player.name);
		printf("[You have %u credits] -> ",player.credits);
		scanf("%d",&choice);

		if((choice < 1) || (choice > 7))
		{
			printf("\n[!!] The number %d is an invalid selection.\n\n",choice);
		}
		else if(choice < 4)		//Otherwise,choice was a game of some sort
		{
			if(choice != last_game)	//If the function ptr isn't set
			{
				if(choice == 1)	//then point it at the selected game
				{
					player.current_game=pick_a_number;
				}
				else if(choice == 2)
				{
					player.current_game=dealer_no_match;
				}
				else
				{
					player.current_game=find_the_ace;
				}
				last_game=choice;//and set last_game
			}
			play_the_game();	 //Play the game
		}
		else if(choice == 4)
		{
			show_highscore();
		}
		else if(choice == 5)
		{
			printf("\nChange user name\n");
			printf("Enter your new name: ");
			input_name();
			printf("Your name has been changed.\n\n");
		}
		else if(choice == 6)
		{
			printf("\nYour account has been reset with 100 credits.\n\n");
			player.credits=100;
		}
	}
	update_player_data();
	printf("\nThanks for playing! Bye.\n");
}
