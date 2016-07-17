#include <stdio.h>
#include <ncurses.h>
#include <ncurses.h>
#include <wiringPi.h>
#include <string.h>
#include <stdlib.h>

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
void print_menu(WINDOW *menu_win, int highlight,char * choices[][2], int n_choices,int startx,int starty,WINDOW * descript_win);
int menus(char * choices[][2],int n_choices);
int manualControl();
void movex(int duration);
void movey(int duration);
void pen(int steps);
int servoControl();
int main()
{
	wiringPiSetup();
       	initscr();
	noecho();
	cbreak();
	start_color();
	clearScreen();
	char  *choices[][2] = { 
			{"Plot File","You select a file to plot and will plot the file on the plotter"},
			{"Manual Control","Will give you full control over the plotter for debuging or demenstration"},
			{"Servo Control","Lets you enter and test different duration of pulses sent to the servo for fine tuning and debugging"},
			{"Exit","Will exit the prgram"},
			};
/* Line buffering disabled. pass on everything */
	int n_choices = sizeof(choices) / (2*sizeof(char *));
	//mvprintw(3,4,"the vale of sizeof(choices):%d\nvale of sieof(char *):%d\nsize of sizeof(*choices):%d\nsize of n_choices:%d",sizeof(choices),sizeof(char *),sizeof(*choices),n_choices);
//	printw("\nthe first two strings are :%s \n and %s",choices[0][0],choices[0][1]);
	int result = menus(choices,n_choices);
	clearScreen();
	if(result == 3)
		servoControl();
	if(result == 2)
	{
		manualControl();	
	}
	
	endwin();
	return 0;
}
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
	count ++;
	if(count != 5&&steps==oldPulse)
		return;
	if(count ==5)
		count = 0;
		
	if(steps == pulse)
		return;
	if(pulse == -1)
	{
		system("echo . > test.txt");
	}
	oldPulse = pulse;
	pulse = steps;
noecho();
	char str1 [70] = "echo 2=";
	char str2 [7];
	char str3 [30] = " > /dev/servoblaster";
	char str4 [100];
	char str5 [7];
	//sprintf(str4,"echo 2=%i >> test.txt ",pulse);
	sprintf(str4,"echo 2=%i > /dev/servoblaster",pulse);
	//mvprintw(4,10,"Value interpreted as:%s",str2);
	//strcpy(str5,str2);
//	refresh();
	//strcat(str4,str1);
//	printw("\n%s\n",str4);

	//strcat(str4,str5);
//	printw("\n%s\n",str4);
//	strcat(str4,str3);
	//move(3,0);
	//clrtoeol();
	//mvprintw(3,15,"\n%s\n",str4);
	////move(14,0);
//	refresh();
	system(str4);
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
		delay(20);
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
				//	pen(450);
				if(pulse == STOPPULSE)
					delay(200);
					pen(value);
					break;
				case KEY_BACKSPACE:
					mvprintw(20,3,"BackSpace pressed!");
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
	int c = getch();
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
				pen(1);
				break;
			case KEY_PPAGE:
				if(DEBUG)
				mvprintw(20,3,"Page down arrrow pressed!");
				pen(-1);
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
	return 0;
}

void clearScreen()
{
	clear();
	init_pair(1,COLOR_BLACK,COLOR_WHITE);
        attron(COLOR_PAIR(1));
        move(0,0);
        for(int i = 0; i < COLS;i++)
                printw(" ");
        mvprintw(0,COLS/2 - 5,"CNC PLOTTER");
        attroff(COLOR_PAIR(1));
	refresh();


}
int menus(char* choices[][2],int n_choices)
	{
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
	
	print_menu(menu_win, highlight,choices,n_choices,startx,starty,descript_win);
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
		print_menu(menu_win, highlight,choices,n_choices,startx,starty,descript_win);
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
	//delwin(menu_win);
	//delwin(descript_win);
	return choice;
}


void print_menu(WINDOW *menu_win, int highlight,char * choices[][2],int n_choices,int startx,int starty,WINDOW * descript_win)
{
	int x, y, i;	

	x = 2;
	y = 2;
	for(i = 0; i < n_choices; ++i)
	{
		if(highlight == i + 1) /* High light the present choice */
		{
			wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i][0]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i][0]);
		++y;
	}
	 descript_win = newwin(HEIGHT, WIDTH+4, starty, startx+WIDTH+2);
	mvwprintw(descript_win, 2,2,"%s",choices[highlight -1][1]);
	wrefresh(menu_win);
	wrefresh(descript_win);
}
