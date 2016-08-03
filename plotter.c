#include <stdio.h>
#include <ncurses.h>
#include <ncurses.h>
#include <wiringPi.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>



#define WIDTH 30
#define HEIGHT 10 

#define DEBUG 0
#define SERVOPIN 1
#define STOPPULSE 150
#define optoSensorX1 15
#define optoSensorX2 8
#define optoSensorY1 18
#define optoSensorY2 18
#define motorXA 16
#define motorXB 16
#define motorYA 15
#define motorYB 15
#define penServo 2    //must be acuroind to servoblaster wiring convention
#define Xstop 18
#define Ystop 18      //the pin that the touch sensor is located on tellign the y asix to stop at 0

#define MAXSIZEX 120
#define MAXSIZEY 120

int count = 0;
int pulse = -1;
int oldPulse = -1;
bool quit = false;
int posX = -1;
int posY = -1;
bool penUp = false;
long timing[(MAXSIZEX>MAXSIZEY)?MAXSIZEX+2:MAXSIZEY+2][4];
void clearScreen();
void print_menu(WINDOW *menu_win, int highlight,char * choices[],char * description[], int n_choices,int startx,int starty,int height,int wordwidth,WINDOW * descript_win);
int menus(char* title,char * choices[], char * description[],int n_choices,int wordwidth);
int manualControl();
void movex(int duration);
void movey(int duration);
void movexy(int xdist,int ydist);
void pen(int steps);
int servoControl();
int plot();
FILE * chooseFile();
void getMaxsizeofDIR(char * dir,int *files,int *length);
int optoRead();
int xyControl();
void initalize();
int safeDelay(int duration);
void allSTOP();
int main()
{
	wiringPiSetup();
       	initscr();
	noecho();
	cbreak();
	start_color();
	while(!quit)
		{
		clearScreen();
		char * choices[] = {"Plot File","Manual Control","Servo Control","Opto Sensor Control","X&Y Control","Exit"};
		char *description[] = {
		"You select a file to plot and will plot the file on the plotter",
		"Will give you full control over the plotter for debuging or demenstration",
		"Lets you enter and test different duration of pulses sent to the servo for fine tuning and debugging",
		"Will read in the togleing of the opto sensor for debuging",
		"Will let you have control how much to move in x and y by ticks more precisly",
		"Will exit the prgram"};
		int result = menus("Hello and Welcome to my creation please sleect a mode", choices,description,6,20);
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
				servoControl();
				break;
			case 4:
				optoControl();
				break;
			case 5:
				xyControl();
				break;
			case 6:
			case -1:
				quit = true;
				break;

		}
	}
	endwin();
	return 0;
}
int optoControl()
{
	clearScreen();
	pinMode(optoSensorX1,INPUT);
	pinMode(optoSensorX2,INPUT);
	bool isOn1 = digitalRead(optoSensorX1);
	bool isOn2 = digitalRead(optoSensorX2); 
	double ticks = 0.0;
	mvprintw(3,4,"Press Enter when ready to start recording the opto sensor, or q to exit!");
	mvprintw(14,10,"Opto Sensor reading:%s\tTicks:%d",(!isOn1)?"Open!!":"Blocked!!!",ticks);
	char c = getch();
	switch(c)
	{
		case '\n':
			break;
		case 'q':
		case 'Q':
			return 0;
			break;
		defautl:
			break;

	}
	keypad(stdscr,TRUE);
	bool isback = false;
	int count = 0;
	while(!quit)
	{
		nodelay(stdscr,0);
				keypad(stdscr,TRUE);
		nodelay(stdscr,1);
		noecho();
		getch();
		getch();
		int c = getch();
		mvprintw(8,5,"Press arrow key to change direction right now we are counting ticks %s, \n\tRight arrow to count up and left arrow to count down",(isback)?"down":"up");
		printw("\n Using pin by Wiring pi 15 witch is 4th pin on the right down. right next to the ground");
		move(14,0);
		clrtoeol();
		mvprintw(14,10,"\tTicks:%d",count);
			
		while(!quit&& c == ERR)
		{
			if(digitalRead(optoSensorX1)!=isOn1)	
			{	
	//			count++;
				isOn1=isOn1;
				if(isback)
					count--;	
				else 
					count++;
				move(14,0);
				clrtoeol();
				mvprintw(14,10,"\tTicks:%d",count);
			
			}
			else if(digitalRead(optoSensorX2)!=isOn2)
			{
				isOn2=!isOn2;
				if(isback)
					count--;	
				else 
					count++;
				move(14,0);
				clrtoeol();
				mvprintw(14,10,"\tTicks:%d",count);
			

			}
			refresh();
			safeDelay(1);
			c = getch();
		}
		switch(c)
		{
			case 'q':
			case 'Q':
				mvprintw(20,3,"Quit has been pressed!");
				return 0;
				break;
			case 'r':
				count = 0;
				break;
			case KEY_RIGHT:
				mvprintw(20,3,"Right arrrow pressed!");
				isback = 0;
				break;
			case KEY_LEFT:
				mvprintw(20,3,"LEFT Arrow pressed!");
				isback = 1;
				break;
			default:
				mvprintw(24, 3, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		refresh();
	}
	





	return 0;
}
void getMaxsizeofDIR(char * dir,int *files,int *length)

{
//printw("\n\nmade it to the beging of get max size of dir\n");
//getch();
	DIR *dp;
	struct dirent *ep;
	*files = 0;
	*length = 0;
//	printw("\njust about to opened the dir just now\n");
//	getch();

	dp = opendir (dir);
//	printw("\nopened the dir just now\n");
//	getch();
	if(dp != NULL)
	{
//	printw("\ndp exists in the if statment!\n");
//	getch();

	while ((ep = readdir (dp))&&!quit)
	{
		
//	printw("\nthe value of d_name is:%s",ep->d_name); getch();
			if(ep->d_name[0]!='.'||ep->d_name[1]=='.')
			{
				++*files;
				int size = strlen(ep->d_name);
				*length = (*length > size)?*length:size;
			}
//			printw("   good!\n");
//			getch();
	}
		(void) closedir(dp);
//		printw("\ncan we close the dir? that is a yes\n\n and the size of files :%d \t and the size of letters is:%d",*files,*length);
//		getch();
	}
	else 
		perror("Could't open the directory");
//printw("\nmade it through this!!\n");
//getch();
}
FILE * chooseFile(char* dir)
{
//	printw("\n\ngoing to try and choosefile on file:%s",dir);
//	getch();
	int files, length;
	getMaxsizeofDIR(dir,&files,&length);
	bool *isDIR = (bool *) malloc(files*sizeof(bool*));
	char **choices = (char**)malloc((files+3)*sizeof(char*));
	int i;
	for(i = 0; i < files; i++)
	{
		choices[i] = (char *)malloc(length*sizeof(char));

	}
	//printw("just created choices in choosefile\n");
	//getch();
	DIR *dp;
	struct dirent *ep;
	dp = opendir (dir);

	//printw("jst opened dp in choosefile\n");
	//getch();
	if(dp != NULL)
	{
//	printw("dp is not null!!!\n");
//	getch();

		isDIR[0] = true;
		strcpy(choices[0],"\\..");
		i = 1;
		clearScreen();
		while(!quit)
		{
			//	printw("\nabout to checck the dir again");
			//	getch();

			if ((ep = readdir (dp))!=NULL)
			{
		//		printw("\tgood!!\n");
	//			getch();
	
		//	printw("\nand the name of value is:%s\n",ep->d_name);
		//	getch();
				if(ep->d_name[0]!='.')
				{
					isDIR[i] = ep->d_type==DT_DIR;
				//printw("number of files is:%d \t and size of length is:%d\t and size of i is:%d",files,length,i);
				//printw("\nand the name of value is:%s",ep->d_name);
				//getch();

					if(isDIR[i])
					{
						strcpy(choices[i],"\\");
						strcat(choices[i],ep->d_name);
					}
					else
						strcpy(choices[i],ep->d_name);
					i++;
			//	printw("\tgood!!\n",ep->d_name);
			//	getch();
				}
			}
			else
			{

				//printw("\nokay we are done abotu to close!!\n");
			//	getch();
				//if(dp!=null)
				//(void) closedir(dp);
			//	printw("\nwe closed!!\n");
			//	getch();
				break;
			}
		}
	//printw("\n right about to call menus\n");
	//getch();
	int result = menus("Select file to be plotted",choices,NULL,files,length+3);
//	printw("\nmade it out of menus!");
//	getch();
	if(result==-1)
		return NULL;
	char  fileName[1024];
//	printw("\n about to copy the string over! to fileName");
//	getch();
//	getcwd(fileName,sizeof(fileName));
	strcpy(fileName,dir);
	if(result==1)
	{
		char ch = 'a';
		int last = 0;
		int count;
		char fileName2[1024];
		for(count = 0;ch !='\0';count++)
		{
			ch = fileName[count];
			if(ch=='/'&&fileName[count+1]!='\0')
				last = count;
		//	printw("\nch:%c\tcount:%d\tlast:%d",ch,count,last);
		//	getch();
		}
		last++;
		for(count = 0;count < last;count++)
		{
			fileName2[count] = fileName[count];
		}
		fileName2[last] = '\0';
//		strncpy(fileName2,fileName,last);
	//	strcat(fileName2,"\0");
//		fileName[last-1] = '\0';
	//	printw("\nchoice was .. and the new dir to open is:%s",fileName2);
	//	getch();
		return chooseFile(fileName2);
	
	}
	int loop;
	char c = 'a';
	strcat(fileName,"\0");
	for(loop = 0;c !='\0';loop++)
	{
	//	printw("\nloop:%d\tc:%c",loop,fileName[loop]);
		 c = fileName[loop];
	}
//	printw("\nbefore we add /: %s\nfileName[loop]:%c",fileName,fileName[loop]);
//	getch();
	if(loop>2&&fileName[loop-2]!='/')	
		strcat(fileName,"/");

//	printw("\nafter we add /: %s",fileName);
//	getch();
	//printw("\ncopied over choices here is fileName:%s",fileName);
	//getch();
//	for(i = 0; i < files; i++)
//	{
//		free(choices[i]);
//	}
//	printw("\njust freed choices[i]! about to free choices");
//	getch();
//	if(choices!=NULL)
	//	free(choices);
//	choices = NULL;
//	printw("\njust freed choicesi");
//	getch();

	if(isDIR[result-1])
	{
//		printw("is a direactory and goign to open:%s\n",fileName);
//		getch();
//		printw("the char at position 1 and 2 are:%c %c\n",fileName[0],fileName[1]);
//		getch();
		//if(isDIR!=NULL)
		//	free(isDIR);
	//	isDIR = NULL;
	//	isDIR = NULL;
	//j	char *target; 
//		for (target = &fileName[0]; *target != '\0'; target++)
//		    *target = *(target+1);
//		*target = '\0';

		strcat(fileName,&(choices[result-1][1]));

//		printw("\n okay so that is a directory that name is:%s and what i want is:%s",fileName,&fileName[2]);
//		getch();
		return chooseFile(fileName);
	}
		strcat(fileName,choices[result-1]);
	//	if(isDIR!=NULL)
	//		free(isDIR);
	//	isDIR=NULL;
		return fopen(fileName,"r"); 
	}
	else
	{
		printw("\ngot an error could not open the directory!\n");
		getch();
		perror("Could't open the directory");
	}
		return fopen("makefile","r"); 

}
int xyControl()
{
	define_key("\033Op", 1000);
	define_key("\033Oq", 1001);
	define_key("\033Or", 1002);
	define_key("\033Os", 1003);
	define_key("\033Ot", 1004);
	define_key("\033Ou", 1005);
	define_key("\033Ov", 1006);
	define_key("\033Ow", 1007);
	define_key("\033Ox", 1008);
	define_key("\033Oy", 1009);

	// non-arrow keypad keys (for macros)
	define_key("\033OM", 1010); // Enter
	define_key("\033OP", 1011); // NumLock
	define_key("\033OQ", 1012); // /
	define_key("\033OR", 1013); // *
	define_key("\033OS", 1014); // -
	define_key("\033Oj", 1015); // *
	define_key("\033Ok", 1016); // +
	define_key("\033Ol", 1017); // +
	define_key("\033Om", 1018); // .
	define_key("\033On", 1019); // .
	define_key("\033Oo", 1020); // -
	clearScreen();
	echo();
	curs_set(2);
	int xdist = 0;
	int ydist = 0;
	while(!quit)
	{    //must be acuroind to servoblaster wiring convention
		move(10,0);
		clrtoeol();
		move(14,0);
		clrtoeol();
		refresh();
		mvprintw(10,5,"Curent Position: X:%d\tY:%d",posX,posY);
		mvprintw(14,5,"Move:");
		mvprintw(14,12,"X:%d",xdist);
		mvprintw(14,22,"Y:%d",ydist);
		move(14,14);
		refresh();
		char valueString [6];
		char c = getch();
		if(c=='q'||c=='Q')
			return 0;
		if(c!='\n')
		{
			printw("     ");
			refresh();
			ungetch(c);
			move(14,14);
			getnstr(valueString,6);
			xdist = atoi(valueString);
		}
		move(14,24);
		c = getch();
		if(c=='q'||c=='Q')
			return 0;
		if(c!='\n')
		{
			printw("     ");
			refresh();
			ungetch(c);
			move(14,24);
			getnstr(valueString,6);
			ydist = atoi(valueString);
		
		}
		move(15,4);
		clrtoeol;
		//getch();
		mvprintw(15,5,"MOVING       X:%d,   Y:%d              ",xdist,ydist);
		refresh();
		posX +=xdist;
		posY +=ydist;
		movexy(xdist,ydist);
	}

}
int plot()
{
	//printw("\nmake it here to line 154! beging of plot\n");
	//getch();
	char dir[1024];
	getcwd(dir,sizeof(dir));
	FILE * fp = chooseFile(dir);
	if(fp==NULL)
		return 1;
		
	printw("made it to after chooseFile()\n");
	getch();
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	if(fp == NULL)
	{
		perror("open failed");
		return 2;
	}
	fclose(fp);
	while(!quit)
	{
		read = getline(&line, &len, fp);
		if(read == -1)
			break;
	}


//	if(line != NULL)
//	free(line);
//	line = NULL;
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
int safeDelay(int duration)
{
	nodelay(stdscr,1);
	clock_t before = clock();
	int c = getch();
	do
	{
		int newc = getch();
		if(c==KEY_EXIT)
		{
			allSTOP();
			quit = true;
			return 1;
		}
//		ungetch(c);      //i dont think i need this line
		delay(1);
	}
	while(((clock() - before )*1000/CLOCKS_PER_SEC)<duration);
	ungetch(c);
	return 0;
}
void allSTOP()
{
	pinMode(motorXA,OUTPUT);
	pinMode(motorXB,OUTPUT);
	pinMode(motorYA,OUTPUT);
	pinMode(motorYB,OUTPUT);
	digitalWrite(motorXA,LOW);
	digitalWrite(motorXB,LOW);
	digitalWrite(motorYA,LOW);
	digitalWrite(motorYB,LOW);
	pen(STOPPULSE);	
}
void movex(int duration)
{
	movexy(duration,0);
}
void movey(int duration)
{
	movexy(0,duration);
}
void movexy(int xdist, int ydist)
{
	if(posX==-1||posY==-1)
		initalize();	
}
void initalize()
{
	pinMode(Xstop,INPUT);
	pinMode(Ystop,INPUT);
	pinMode(motorXA,OUTPUT);
	pinMode(motorXB,OUTPUT);
	pinMode(motorYA,OUTPUT);
	pinMode(motorYB,OUTPUT);
	pinMode(optoSensorX1,INPUT);
	pinMode(optoSensorX2,INPUT);
	pinMode(optoSensorY1,INPUT);
	pinMode(optoSensorY2,INPUT);
	digitalWrite(motorXA,LOW);
	digitalWrite(motorXB,LOW);
	digitalWrite(motorYA,LOW);
	digitalWrite(motorYB,LOW);
	clearScreen();
	keypad(stdscr,TRUE);
	while(!quit)
	{
		mvprintw(5,10,"Please use Page Up and Page Down Arrows to move the pen to just touching the page,When centered hit 'q'");
		pen(STOPPULSE);
		int c = getch();
		switch(c)
		{
			case KEY_NPAGE:
				pen(250);
				safeDelay(30);
				break;
			case KEY_PPAGE:
				pen(50);
				safeDelay(30);
				break;
			case 'q':
			case 'Q':
				pen(50);
				safeDelay(200);
			defaut:
				break;
		}
		
	}
	penMove(1);
	if(digitalRead(Xstop))
	{
		digitalWrite(motorXA,HIGH);	
		if(safeDelay(500))
			return 1;
		digitalWrite(motorXA,LOW);
	}
	digitalWrite(motorXB,HIGH);
	while(!quit&&!digitalRead(Xstop))
		safeDelay(1);
	posX=0;
	digitalWrite(motorXB,LOW);
	if(digitalRead(Ystop))
	{
		digitalWrite(motorYA,HIGH);	
		if(safeDelay(500);
		digitalWrite(motorYA,LOW);
	}
	digitalWrite(motorYB,HIGH);
	while(!quit&&!digitalRead(Ystop))
		safeDelay(1);
	posY=0;
	digitalWrite(motorYB,LOW);
	clock_t before = clock();
	bool OptoX1 = digitalRead(optoSensorX1);		
	bool OptoX2 = digitalRead(optoSensorX2);
	digitalWrite(motorXA,HIGH);
	while(!quit&&posX<=MAXSIZEX)
	{
		if(OptoX1!=digitalRead(optoSensorX1))
			OptoX1 = !OptoX1;
		else if(OptoX2!=digitalRead(optoSensorX2))
			OptoX2 = !OptoX2;
		else	
			continue;
		timing[posX][0] = (long) clock();	
		posX++;
		if(safeDelay(1);
	}
	digitalWrite(motorXA,LOW);
	digitalWrite(motorXB,HIGH);
	while(!quit&&posX>=0)
	{
		if(OptoX1!=digitalRead(optoSensorX1))
			OptoX1 = !OptoX1;
		else if(OptoX2!=digitalRead(optoSensorX2))
			OptoX2 = !OptoX2;
		else	
			continue;
		timing[posX][1] = (long) clock();	
		posX--;
		if(safeDelay(1);
	}
	digitalWrite(motorXB,LOW);
	before = clock();
	bool OptoY1 = digitalRead(optoSensorY1);		
	bool OptoY2 = digitalRead(optoSensorY2);
	digitalWrite(motorYA,HIGH);
	while(!quit&&posX<=MAXSIZEX)
	{
		if(OptoX1!=digitalRead(optoSensorY1))
			OptoY1 = !OptoY1;
		else if(OptoY2!=digitalRead(optoSensorY2))
			OptoY2 = !OptoY2;
		else	
			continue;
		timing[posY][3] = (long) clock();	
		posY++;
		if(safeDelay(1);
	}
	digitalWrite(motorYA,LOW);
	digitalWrite(motorYB,HIGH);
	while(!quit&&posY>=0)
	{
		if(OptoY1!=digitalRead(optoSensorY1))
			OptoY1 = !OptoY1;
		else if(OptoY2!=digitalRead(optoSensorY2))
			OptoY2 = !OptoY2;
		else	
			continue;
		timing[posY][4] = (long) clock();	
		posY--;
		if(safeDelay(1);
	}
	digitalWrite(motorYB,LOW);



}
void penMove(bool goUp)
{
	char output[100];
	if(goUp)
	{
		sprintf(output,"echo %i=%i > /dev/servoblaster",penServo,250);
		penUp = true;
	}
	else
	{
		sprintf(output,"echo %i=%i > /dev/servoblaster",penServo,50);
		penUp = false;
	}
	system(output);
	if(safeDelay(1000);
	sprintf(output,"echo %i=%i > /dev/servoblaster",penServo,STOPPULSE);
	system(output);
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
	sprintf(output,"echo %i=%i > /dev/servoblaster",penServo,pulse);
	system(output);
}
int servoControl()
{
	clearScreen();
	mvprintw(3,0,"Servo control to manual mess with the timing of the servo, enter a value then hit enter and use right arrow to execute the value and hit backspace to got back to change the value");
	refresh();
	keypad(stdscr,TRUE);
	int choice = 0;
	while(!quit)
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
		while(!quit&&choice == 0)
		{
			keypad(stdscr,TRUE);
			nodelay(stdscr,1);
		//	noecho();
			int c = getch();
			while(!quit&& c == ERR)
			{

				pen(STOPPULSE);
				safeDelay(15);
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
					safeDelay(30);
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
	while(!quit)
	{
		nodelay(stdscr,1);
		//	noecho();
			int c = getch();
			while(!quit&& c == ERR)
			{

				pen(STOPPULSE);
				safeDelay(15);
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
				penMove(1);
				break;
			case KEY_PPAGE:
				if(DEBUG)
				mvprintw(20,3,"Page down arrrow pressed!");
				penMove(0);
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
int menus(char * title,char * choices[], char * description[],int n_choices,int wordwidth)
	{
	clearScreen();
	WINDOW *menu_win;
	WINDOW *descript_win;
	int highlight = 1;
	int choice = 0;
	int c;
	int height = (n_choices + 4<HEIGHT)?HEIGHT:n_choices+4;
	int width = (wordwidth+4<WIDTH)?WIDTH:wordwidth;
	int startx = (80 - width) / 3;
	int starty = 4+(24 - height) / 3;

	menu_win = newwin(height, width, starty, startx);
	keypad(menu_win, TRUE);
        mvprintw(2,2,"%s",title);
	//mvprintw(4, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	
	print_menu(menu_win, highlight,choices,description,n_choices,startx,starty,height,width,descript_win);
	while(!quit)
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
			case 'q':
				return -1;
			default:
				break;
		}
		print_menu(menu_win, highlight,choices,description,n_choices,startx,starty,height,width,descript_win);
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


void print_menu(WINDOW *menu_win, int highlight,char * choices[],char *description[],int n_choices,int startx,int starty,int height,int width,WINDOW * descript_win)
{
	int x, y, i;	
	init_pair(4, COLOR_RED, COLOR_BLACK);
	curs_set(0);
	init_pair(5, COLOR_RED, COLOR_WHITE);
	x = 2;
	y = 2;
	for(i = 0; i < n_choices; ++i)
	{
		if(highlight == i + 1) /* High light the present choice */
		{
			if(choices[i][0]=='\\')
			{
				wattron(menu_win,COLOR_PAIR(5));
				mvwprintw(menu_win, y, x, "%s", ++choices[i]);
				wattroff(menu_win,COLOR_PAIR(5));	
				choices[i]--;
			}
			else{

			wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
			}
		}
		else
		{
		
			if(choices[i][0]=='\\')
			{
				wattron(menu_win,COLOR_PAIR(4));
				mvwprintw(menu_win, y, x, "%s", ++choices[i]);
				wattroff(menu_win,COLOR_PAIR(4));	
				choices[i]--;
			}
			else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		}
		++y;
	
	}  
//	mvprintw(2,0,"We are at the suspcected area at 496\n");
//	getch();
	if(description!=NULL)
	{
	 descript_win = newwin(height, width+4, starty, startx+width+2);
	mvwprintw(descript_win, 2,2,"%s",description[highlight -1]);
	wrefresh(descript_win);
	}
	wrefresh(menu_win);
}
