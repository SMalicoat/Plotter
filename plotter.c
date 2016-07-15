#include <stdio.h>
#include <dirent.h>
#include <wiringPi.h>
#include <unistd.h>
#include <ncurses.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
/*
	printf(RED "red\n" RESET);
	printf(GRN "green\n" RESET);
	printf(YEL "yellow\n" RESET);
	printf(BLU "blue\n" RESET);
	printf(MAG "magenta\n" RESET);
	printf(CYN "cyan\n" RESET);
	printf(WHT "white\n" RESET);
*/	      


bool quit = false;
void clearScreen()
{
	const char* CLEAR_SCREE_ANSI = "\e[1;1H\e[2J";
	write(STDOUT_FILENO,CLEAR_SCREE_ANSI,12);
}

int basicScreen()
{
	clearScreen();
	init_pair(1,COLOR_BLACK,COLOR_WHITE);
	attron(COLOR_PAIR(1));
	move(0,0);
	for(int i = 0; i < COLS;i++)
		printw(" ");
	mvprintw(0,COLS/2 - 5,"CNC PLOTTER");
	attroff(COLOR_PAIR(1));
	int letters;
	mvprintw(4,COLS/2-56,"Hello and welcome to my creation. To start please select with arrow keys what mod you would like to start with.%n",&letters);
	//printw(": %i",letters);
	attron(COLOR_PAIR(1));
	mvprintw(15,COLS/2-9,"[1] Plot File");
	attroff(COLOR_PAIR(1));
	mvprintw(16,COLS/2-9,"[2] Manual Control");
	mvprintw(17,COLS/2-9,"[3] Exit");
	move(LINES * .9,8);
	int highlight = 0;
	bool selected = false;
	while(!quit)
	{
		int c = getch();
		switch(c)
		{
			case KEY_UP:
				highlight--;
				if(highlight<1)
					highlight = 1;
				break;
			case KEY_DOWN:
				highlight++;
				if(highlight > 3)
					highlight = 3;
				break;
			case 113:
			case 81:
		//	case 27:
				quit = true;
				break;
			
			case 10:
				selected = true;
				break;
			default:
				printw("did not reconize the command: %c",c);
				break;
		}
		attron(COLOR_PAIR(1));
		switch(highlight)
		{
			case 1:
				mvprintw(15,COLS/2-9,"[1] Plot File");
				break;
			case 2:
				mvprintw(16,COLS/2-9,"[2] Manual Control");
				break;
			case 3:
				mvprintw(17,COLS/2-9,"[3] Exit");
				break;
			default:
			break;
		}
		attroff(COLOR_PAIR(1));

	}
	if(quit)
		return 0;
	printw("you selected option: %i",highlight);
	refresh();
	return 1;
}
int main()
{

	//wiringPiSetupGpio(): //need to do this once i start working with the
		//pi need to look into what pin number sceme to use

	initscr();	
	start_color();
	noecho();
	cbreak();
/*	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen ("sampleGcode.txt","r");
	if(fp == NULL)
	{
		perror("open failed");
		return 2;
	}
	while((read = getline(&line, &len, fp)) != -1)
	{
		printw("%s",line);
		refresh();
	}
	
	fclose(fp);*/
//	if(line)
//		free(line);
	
/*	DIR *dp;
	struct dirent *ep;
	dp = opendir ("./");
	if(dp != NULL)
	{
		while (ep = readdir (dp))
			if(ep->d_name[0]!='.')
				printw("%s  ",ep->d_name);
		(void) closedir(dp);
		refresh();
	}
	else 
		perror("Could't open the directory");
		*/
	basicScreen();
	while(true)
	{
	}
	getch();	
	endwin();
	return 0;
}

