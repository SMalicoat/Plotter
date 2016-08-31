#include <stdio.h>
#include <ncurses.h>
#include <ncurses.h>
#include <wiringPi.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>



#define WIDTH 30
#define HEIGHT 10 
#define TOPMENU 0
#define FILESELECT 1
#define PLOTDISPLAY 2

#define DEBUG 0
#define SERVOPIN 1
#define STOPPULSE 150


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
int count = 0;
int pulse = -1;
int oldPulse = -1;
bool quit = false;
void clearScreen();
void print_menu(WINDOW *menu_win, int highlight,char * choices[],char * description[], int n_choices,int startx,int starty,WINDOW * descript_win);
int menus(char * choices[], char * description[],int n_choices);
int manualControl();
void movex(int duration);
void movey(int duration);
void pen(int steps);
int servoControl();
int plot();
FILE * chooseFile();
void getMaxsizeofDIR(int *files,int *length);
int main()
{
	wiringPiSetup();
       	initscr();
	noecho();
	cbreak();
	start_color();
	while(1)
		{
		clearScreen();
		char * choices[] = {"Plot File","Manual Control","Servo Control","Exit"};
		char *description[] = {
		"You select a file to plot and will plot the file on the plotter",
		"Will give you full control over the plotter for debuging or demenstration",
		"Lets you enter and test different duration of pulses sent to the servo for fine tuning and debugging",
		"Will exit the prgram"};
		int result = menus(choices,description,4);
		clearScreen();
		switch (result)
		{
			case 1:
			/*	FILE * fp;
				char * line = NULL; 
				size_t len = 0;
				ssize_t read;
				fp = fopen ("sampleGcode.txt","r");*/
				plot();

				break;
			case 2:
				manualControl();
				break;
			case 3:
				servoControl:
				break;
			case 4:
				quit = true;
				break;

		}
	}
	endwin();
	return 0;
}
void getMaxsizeofDIR(int *files,int *length)

{
printw("\n\nmade it to the beging of get max size of dir\n");
getch();
	DIR *dp;
	struct dirent *ep;
	*files = 0;
	*length = 0;
	dp = opendir ("./");
	printw("\nopened the dir just now\n");
	if(dp != NULL)
	{
	printw("\ndp exists in the if statment!\n");

	while (ep = readdir (dp))
			if(ep->d_name[0]!='.'||ep->d_name[1]=='.')
			{
				*files++;
				int size = strlen(ep->d_name);
				*length = (*length > size)?*length:size;
			}
		(void) closedir(dp);
	}
	else 
		perror("Could't open the directory");
}
FILE * chooseFile()
{
	int files, length;
	getMaxsizeofDIR(&files,&length);
	char **choices = (char**)malloc(files*sizeof(char*));
	int i;
	for(i = 0; i < files; i++)
	{
		choices[i] = (char *)malloc(length*sizeof(char));
	}
	printw("just created choices in choosefile\n");
	DIR *dp;
	struct dirent *ep;
	dp = opendir ("./");
	if(dp != NULL)
	{
		i = 0;
		while (ep = readdir (dp))
			if(ep->d_name[0]!='.'&&ep->d_name[1]!='.')
			{
				strcpy(choices[i],ep->d_name);
				i++;
			}
		int result = menus(choices,NULL,files);
		(void) closedir(dp);
		refresh();
	}
	else 
		perror("Could't open the directory");
		return fopen("makefile","r"); 

}
int plot()
{
	printw("make it here to line 154! beging of plot\n");
	getch();
	FILE * fp = chooseFile();
	printw("made it to after chooseFile()\n");
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	if(fp == NULL)
	{
		perror("open failed");
		return 2;
	}
	fclose(fp);
	while(1)
	{
		read = getline(&line, &len, fp);
		if(read == -1)
			break;
	}


	if(line)
	free(line);
	return 0;
}

	
//switch(option)
//	{
//		case PLOTDISPLAY:
//			char *choices[] = { 
//				"Plot DISPLAY",
//				"INSTERT HERE",
//				"Exit",
//				};
//		break;
//		case FILESELECT:
//			char *choices[] = { 
//				"FILE SELECT",
//				"INSTERT HERE",
//				"Exit",
//				};
//		break;
//		default:
//			char *choices[] = { 
//				"Plot File",
//				"Manual Control",
//				"Exit",
//				};

//			//		break;
//	}
void movex(int duration)
{
}
void movey(int duration)
{
}
void pen(int steps)
{
		
	if(steps == pulse)
		return;
	if(pulse == -1)
	{
//		system("~/servoBlaster/servod");
	}
	noecho();
	char str2 [7];
	char output [100];
	sprintf(output,"echo 2=%i > /dev/servoblaster",pulse);
	system(output);
}
int servoControl()
{
	clearScreen();
	mvprintw(3,0,"Servo control to manual mess with the timing of the servo, enter a value then hit enter and use right arrow to execute the value and hit backspace to got back to change the value");
	refresh();
	keypad(stdscr,TRUE);
	int choice = 0;
	while(1)
	{
		echo();
		nodelay(stdscr,0);
		move(13,0);
		clrtoeol();
		mvprintw(13,15,"Value to run at:");
		char valueString [5];
		getnstr(valueString,5);
		int value = atoi(valueString);
		choice = 0;
		mvprintw(14,12,"Value entered: %i",value);
		while(choice == 0)
		{
			keypad(stdscr,TRUE);
			nodelay(stdscr,1);
		//	noecho();
			int c = getch();
			while( c == ERR)
			{

				pen(STOPPULSE);
				delay(15);
				c = getch();
			}
			switch(c)
			{
				case 'q':
				case 'Q':
					mvprintw(20,3,"Quit has been pressed!");
					choice = 1;
					break;
				case KEY_RIGHT:
					mvprintw(20,3,"Right arrrow pressed!");
					pen(value);
					delay(30);
					break;
				case KEY_BACKSPACE:
					mvprintw(20,3,"BackSpace pressed!");
					clrtoeol();
					choice = 2;
					break;
				default:
					mvprintw(24, 3, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
					refresh();
					break;
			}
			refresh();
			if(choice != 0)	/* User did a choice come out of the infinite loop */
				break;
		}
		if(choice == 1)
			break;
	}
	pen(150);
	return 0;
}

int manualControl()
{
	clearScreen();
	mvprintw(3,0,"\tUP--Up Arrow\tDown--Down Arrow\tLeft--Left Arrow\tRight--Right Arrow\t\t\t\tPen Up--Page Up\t\tPen Down--Page Down");
	refresh();
	keypad(stdscr,TRUE);
	int choice = 0;
	while(1)
	{
		nodelay(stdscr,1);
		//	noecho();
			int c = getch();
			while( c == ERR)
			{

				pen(STOPPULSE);
				delay(15);
				c = getch();
			}
		switch(c)
		{
			case KEY_UP:
				if(DEBUG)
				mvprintw(20,3,"Up arrrow pressed!");
				movex(50);
				break;
			case KEY_DOWN:
				if(DEBUG)
				mvprintw(20,3,"Down arrrow pressed!");
				movex(-50);
				break;
			case 'q':
			case 'Q':
				if(DEBUG)
				mvprintw(20,3,"Quit has been pressed!");
				choice = 1;
				break;
			case KEY_LEFT:
				if(DEBUG)
				mvprintw(20,3,"Left arrrow pressed!");
				movey(-50);
				break;
			case KEY_RIGHT:
				if(DEBUG)
				mvprintw(20,3,"Right arrrow pressed!");
				movey(50);
				break;
			case KEY_NPAGE:
				if(DEBUG)
				mvprintw(20,3,"Page Up arrrow pressed!");
				pen(250);
				delay(30);
				break;
			case KEY_PPAGE:
				if(DEBUG)
				mvprintw(20,3,"Page down arrrow pressed!");
				pen(50);
				delay(30);
				break;
			default:
				mvprintw(24, 3, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		refresh();
		if(choice != 0)	/* User did a choice come out of the infinite loop */
			break;

	}

	pen(150);
	return 0;
}

void clearScreen()
{
	clear();
	init_pair(1,COLOR_BLACK,COLOR_WHITE);
        attron(COLOR_PAIR(1));
        move(0,0);
	int i;
        for(i = 0; i < COLS;i++)
                printw(" ");
        mvprintw(0,COLS/2 - 5,"CNC PLOTTER");
        attroff(COLOR_PAIR(1));
	refresh();


}
int menus(char * choices[], char * description[],int n_choices)
	{
	clearScreen();
	WINDOW *menu_win;
	WINDOW *descript_win;
	int highlight = 1;
	int choice = 0;
	int c;

	int startx = (80 - WIDTH) / 3;
	int starty = 4+(24 - HEIGHT) / 3;

	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
        mvprintw(2,2,"Hello and welcome to my creation. To start please select with the arrow keys");
	//mvprintw(4, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	
	print_menu(menu_win, highlight,choices,description,n_choices,startx,starty,descript_win);
	while(1)
	{
	c = wgetch(menu_win);
		switch(c)
		{
			case KEY_UP:
				if(highlight == 1)
					highlight = n_choices;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == n_choices)
					highlight = 1;
				else 
					++highlight;
				break;
			case 10:
				choice = highlight;
				break;
			default:
				mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		print_menu(menu_win, highlight,choices,description,n_choices,startx,starty,descript_win);
		if(choice != 0)	/* User did a choice come out of the infinite loop */
			break;
	}	
//	mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
	//clrtoeol();
	refresh();
	//werase(menu_win);
	//werase(descript_win);
	clear();
	refresh();
	clearScreen();
	//delwin(menu_win);
	//delwin(descript_win);
	return choice;
}


void print_menu(WINDOW *menu_win, int highlight,char * choices[],char *description[],int n_choices,int startx,int starty,WINDOW * descript_win)
{
	int x, y, i;	

	x = 2;
	y = 2;
	for(i = 0; i < n_choices; ++i)
	{
		if(highlight == i + 1) /* High light the present choice */
		{
			wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	
	}
	if(description!=NULL)
	{
	 descript_win = newwin(HEIGHT, WIDTH+4, starty, startx+WIDTH+2);
	mvwprintw(descript_win, 2,2,"%s",description[highlight -1]);
}
	wrefresh(menu_win);
	wrefresh(descript_win);
}
