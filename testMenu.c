#include <stdio.h>
#include <ncurses.h>

#define WIDTH 30
#define HEIGHT 10 

#define TOPMENU 0
#define FILESELECT 1
#define PLOTDISPLAY 2

int startx = 0;
int starty = 0;


void print_menu(WINDOW *menu_win, int highlight,char ** choices, int n_choices);
int menus(char ** choices,int n_choices);
int main()
{
	initscr();
	clear();
	noecho();
	cbreak();
	char *choices[] = { 
			"Plot File",
			"Manual Control",
			"Exit",
			};
/* Line buffering disabled. pass on everything */
	int n_choices = sizeof(choices) / sizeof(char *);
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
int menus(char ** choices,int n_choices)
	{
	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;

	startx = (80 - WIDTH) / 2;
	starty = 4+(24 - HEIGHT) / 2;

	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(4, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	
	print_menu(menu_win, highlight,choices,n_choices);
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
		print_menu(menu_win, highlight,choices,n_choices);
		if(choice != 0)	/* User did a choice come out of the infinite loop */
			break;
	}	
	mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
	clrtoeol();
	refresh();
	delwin(menu_win);
	return choice;
}


void print_menu(WINDOW *menu_win, int highlight,char ** choices,int n_choices)
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
	wrefresh(menu_win);
}
