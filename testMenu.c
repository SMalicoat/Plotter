#include <stdio.h>
#include <ncurses.h>

#define WIDTH 30
#define HEIGHT 10 

#define TOPMENU 0
#define FILESELECT 1
#define PLOTDISPLAY 2




void print_menu(WINDOW *menu_win, int highlight,char * choices[][2], int n_choices,int startx,int starty);
int menus(char * choices[][2],int n_choices);
int main()
{
	initscr();
	clear();
	noecho();
	cbreak();
	char  *choices[][2] = { 
			{"Plot File","You select a file to plot and will plot the file on the plotter"},
			{"Manual Control","Will give you full control over the plotter for debuging or demenstration"},
			{"Exit","Will exit the prgram"},
			};
/* Line buffering disabled. pass on everything */
	int n_choices = sizeof(choices) / (2*sizeof(char *));
	//mvprintw(3,4,"the vale of sizeof(choices):%d\nvale of sieof(char *):%d\nsize of sizeof(*choices):%d\nsize of n_choices:%d",sizeof(choices),sizeof(char *),sizeof(*choices),n_choices);
//	printw("\nthe first two strings are :%s \n and %s",choices[0][0],choices[0][1]);
	menus(choices,n_choices);
	endwin();
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
int menus(char* choices[][2],int n_choices)
	{
	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;

	int startx = (80 - WIDTH) / 3;
	int starty = 4+(24 - HEIGHT) / 3;

	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(4, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	
	print_menu(menu_win, highlight,choices,n_choices,startx,starty);
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
		print_menu(menu_win, highlight,choices,n_choices,startx,starty);
		if(choice != 0)	/* User did a choice come out of the infinite loop */
			break;
	}	
	mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
	clrtoeol();
	refresh();
	delwin(menu_win);
	return choice;
}


void print_menu(WINDOW *menu_win, int highlight,char * choices[][2],int n_choices,int startx,int starty)
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
	WINDOW *descript_win = newwin(HEIGHT, WIDTH, starty, startx+WIDTH+2);
	mvwprintw(descript_win, 0,0,"%s",choices[highlight -1][1]);
	wrefresh(menu_win);
	wrefresh(descript_win);
}
