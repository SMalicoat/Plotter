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
#define STOPPULSE 150   //all of the pin numbers are wiringPI standard!!
#define optoSensorX1 4 
#define optoSensorX2 5
#define optoSensorY1 6
#define optoSensorY2 18
#define motorXA 7 
#define motorXB 0 
#define motorYA 2 
#define motorYB 3 
#define penServo 1   
#define Xstop 17 
#define Ystop 19      //the pin that the touch sensor is located on tellign the y asix to stop at 0

#define MAXSIZEX 120
#define MAXSIZEY 120

int count = 0;
int pulse = -1;
int oldPulse = -1;
bool quit = false;
int posX = -1;
int posY = -1;
bool penUp = false;
bool optoValue[4];
int config = false;
double penAffect = -1;
long timing[(MAXSIZEX>MAXSIZEY)?MAXSIZEX+2:MAXSIZEY+2][4];
void clearScreen();
void print_menu(WINDOW *menu_win, int highlight,char * choices[],char * description[], int n_choices,int startx,int starty,int height,int wordwidth,WINDOW * descript_win);
int menus(char* title,char * choices[], char * description[],int n_choices,int wordwidth);
int manualControl();
void movex(int duration);
void movey(int duration);
void movexy(int Xdist,int Ydist);
void pen(int steps);
void penMove(bool goUp);
int servoControl();
int plot();
FILE * chooseFile();
void getMaxsizeofDIR(char * dir,int *files,int *length);
int optoControl();
int xyControl();
void initalize();
int safeDelay(int duration);
void allSTOP();
int didTick(int checkX,int checkY);
void power(int pin,int percent);
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
				return 0; 
				break;

		}
	}
	endwin();
	return 0;
}
int didTick(int checkX,int checkY) //return 0 if no change. return 1 if x changed return -1 if y changed
{
	if(!config)
	{
	config = true;
		mvprintw(4,0,"OptoValue is null so inializing!!!! HIt enter to continue!");
		
		pinMode(optoSensorX1,INPUT);
		pinMode(optoSensorX2,INPUT);
		pinMode(optoSensorY1,INPUT);
		pinMode(optoSensorY2,INPUT);
		optoValue[0]=digitalRead(optoSensorX1);
		optoValue[1]=digitalRead(optoSensorX2);
		optoValue[2]=digitalRead(optoSensorY1);
		optoValue[3]=digitalRead(optoSensorY2);
	//	printw("\n the value of sensor 1 and 2 are:::%d\n%d",optoValue[0],optoValue[1]);
	//	refresh();
	//	while(getch()!= ERR)
	//	{
	//		delay(1);
	//	}
		return 0;
	}
	if(checkX&&(optoValue[0]!=digitalRead(optoSensorX1)))
	{
		optoValue[0]=!optoValue[0];
		return 1;
	}
	else if(checkX&&(optoValue[1]!=digitalRead(optoSensorX2)))
	{
		optoValue[1]=!optoValue[1];
		return 1;
	}
	if(checkY&&(optoValue[2]!=digitalRead(optoSensorY1)))
	{
		optoValue[2]=!optoValue[2];
		return -1;
	}
	else if(checkY&&(optoValue[3]!=digitalRead(optoSensorY2)))
	{
		optoValue[3]=!optoValue[3];
		return -1;
	}
	return 0;
}
int optoControl()
{ 
	clearScreen();
	
	keypad(stdscr,TRUE);
	bool isback = false;
	int xticks = 0;
	int yticks = 0;
	while(!quit)
	{
		keypad(stdscr,TRUE);
		nodelay(stdscr,1);
		noecho();
		getch();
		getch();
		int c = getch();
		mvprintw(8,5,"Press arrow key to change direction right now we are counting ticks %s, \n\tRight arrow to count up and left arrow to count down",(isback)?"down":"up");
		move(14,0);
		clrtoeol();
		mvprintw(14,10,"\tX-Ticks:%d\tY-Ticks:%d",xticks,yticks);
		refresh();
			

		while(!quit  && c == ERR)
		{
			int result = didTick(1,1); 	
			delay(1);
//`		safeDelay(1);
			c=getch();
			if(result==0)
				continue;
			if(result==1)
				xticks=(isback)?xticks-1:xticks+1;
			if(result==-1)
				yticks=(isback)?yticks-1:yticks+1;
		
			move(14,0);
			clrtoeol();
			mvprintw(14,10,"\tX-Ticks:%d\tY-Ticks:%d",xticks,yticks);
			refresh();
		}	
		move(20,0);
		clrtoeol();

		switch(c)
		{
			case 'q':
			case 'Q':
				mvprintw(20,3,"Quit has been pressed!");
				return 0;
				break;
			case 'r':
				xticks = 0;
				yticks = 0;
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
		delay(10);
//		safeDelay(10);
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
	int Xdist = 0;
	int Ydist = 0;
	while(!quit)
	{    //must be acuroind to servoblaster wiring convention
		move(10,0);
		clrtoeol();
		move(14,0);
		clrtoeol();
		refresh();
		mvprintw(10,5,"Curent Position: X:%d\tY:%d",posX,posY);
		mvprintw(14,5,"Move:");
		mvprintw(14,12,"X:%d",Xdist);
		mvprintw(14,22,"Y:%d",Ydist);
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
			Xdist = atoi(valueString);
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
			Ydist = atoi(valueString);
		
		}
		move(15,4);
		clrtoeol;
		//getch();
		mvprintw(15,5,"MOVING       X:%d,   Y:%d              ",Xdist,Ydist);
		refresh();
		posX +=Xdist;
		posY +=Ydist;
		movexy(Xdist,Ydist);
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
	if(quit)
		return 1;
	nodelay(stdscr,1);
	clock_t before = clock();
	//int c = getch();
	do
	{
		int newc = getch();
		if(newc==27&&getch()==-1)
		{
			allSTOP();
			quit = true;
			return 1;
		}
		
//		ungetch(c);      //i dont think i need this line
		delay(1);
	}
	while(((clock() - before )*(1000/CLOCKS_PER_SEC))<duration);
//	ungetch(c);
	return 0;
}
void allSTOP()
{
	pinMode(motorXA,OUTPUT);
	pinMode(motorXB,OUTPUT);
	pinMode(motorYA,OUTPUT);
	pinMode(motorYB,OUTPUT);
	power(motorXA,0);
	power(motorXB,0);
	power(motorYA,0);
	power(motorYB,0);
	pen(STOPPULSE);	
}
void power(int pin,int percent)
{
	char output[100];
	sprintf(output,"echo %i=%i%% > /dev/servoblaster",pin,percent);
	system(output);
}
void movex(int Xdist)
{
	movexy(Xdist,0);
}
void movey(int Ydist)
{
	movexy(0,Ydist);
}
void movexy(int Xdist, int Ydist)// note this is moving relative to where we are
{
	if(posX==-1||posY==-1)
		initalize();	

	if(posX+Xdist>MAXSIZEX)              //make sure we dont go out of bounds too high
		Xdist = MAXSIZEX-posX;
	if(posX+Xdist<0)                    //make sure we dont go out of bound too low
		Xdist = -posX;
	if(posY+Ydist>MAXSIZEY)
		Ydist = MAXSIZEY-posY;
	if(posY+Ydist<0)
		Ydist = -posY;

	if(penUp)
	{
		bool foundX = false;
		bool foundY = false;
		if(Xdist>10)    //need to go fowards 
		{
			power(motorXA,100);
			power(motorXB,0);
		}else if (Xdist<-10)  //need to go backword 
		{
			power(motorXA,0);
			power(motorXB,100);
		}else if(abs(posX/10-(posX+Xdist)/10)>0)
		{
			if(Xdist>0)
				power(motorXA,50);
			else
				power(motorXB,50);
			while(didTick(1,0)!=1)
			{
				safeDelay(1);
			}
			power(motorXA,0);
			power(motorXB,0);
			int moved;
			if(Xdist>0)
			{
				moved = 10 - posX % 10;
				posX  += moved;
				Xdist -= moved;
				//increment the posX for we moved the machine
				//decrease the value that we still have to go 
			}
			else
			{
				moved = posX % 10;
				moved = (moved == 0)?10:moved;
				posX  -= moved;
				Xdist += moved;
			}
			foundX = true;
		
		}else                //already at the correct x point
		{
			power(motorXA,0);
			power(motorXB,0);
			foundX = true;
		}
		if(Ydist>10)
		{                      //need to go fowards
			power(motorYA,100);
			power(motorYB,0);

		}else if (Ydist<-10)  //need to go backwords
		{
			power(motorYA,0);
			power(motorYB,100);
		}else if(abs(posY/10-(posY+Ydist)/10)>0)
		{
			if(Ydist>0)
				power(motorYA,50);
			else
				power(motorYB,50);
			while(didTick(0,1)!=-1)
			{
				safeDelay(1);
			}
			power(motorYA,0);
			power(motorYB,0);
			int moved;
			if(Ydist>0)
			{
				moved = 10 - posY % 10;
				posY  += moved;
				Ydist -= moved;
				//increment the posX for we moved the machine
				//decrease the value that we still have to go 
			}
			else
			{
				moved = posY % 10;
				moved = (moved == 0)?10:moved;
				posY  -= moved;
				Ydist += moved;
			}
			foundY = true;
		}else              //alrady at the correct pint
		{
			power(motorYA,0);
			power(motorYB,0);
			foundY = true;
		}
		while(!foundX||!foundY)
		{
			int result = didTick(!foundX,!foundY);		
			int moved;
			if(!foundX)
			{
				if(result==1)
				{
					if(Xdist>0)
					{
						moved = 10 - (posX % 10);
						posX  += moved;
						Xdist -= moved;
						//increment the posY for we moved the machine
						//decrease the value that we still have to go 
					}
					else
					{
						moved = posX%10;
						moved = (moved==0)?10:moved;	
						posX  -= moved;
						Xdist += moved;
					}
					if(abs(Xdist)<=10)
						power(motorXA,0);
						power(motorXB,0);
						foundX = true;
				}

			}
			if(!foundY)
			{
				if(result==-1)
				{
					if(Ydist>0)
					{
						moved = 10 - (posY % 10);
						posY  += moved;
						Ydist -= moved;
						//increment the posY for we moved the machine
						//decrease the value that we still have to go 
					}
					else
					{
						moved = (posY % 10);
						moved = (moved==0)?10:moved;	
						posY  -= moved;
						Ydist += moved;
					}
					if(abs(Ydist)<=10)
					{
						power(motorYA,0);
						power(motorYB,0);
						foundY = true;
					}
				}
			}
			safeDelay(1);
		}
		if(Xdist!=0 && posX % 10 == 0 && abs(Xdist) % 10 == 0) //if we are lucky and have to move exactly one tick
		{
			if(Xdist>0)
				power(motorXA,50);
			else
				power(motorXB,50);
			while(didTick(1,0)!=1)
			{
				safeDelay(1);
			}
			power(motorXA,0);
			power(motorXB,0);
			if(Xdist>0)
			{
				posX  += 10;
				Xdist -= 10;
				//increment the posX for we moved the machine
				//decrease the value that we still have to go 
			}
			else
			{
				posX  -= 10;
				Xdist += 10;
			}
		}
		if(Ydist!=0 && posY % 10 == 0 && abs(Ydist) % 10 == 0)
		{
			if(Ydist>0)
				power(motorYA,50);
			else
				power(motorYB,50);
			while(didTick(0,1)!=-1)
			{
				safeDelay(1);
			}
			power(motorYA,0);
			power(motorYB,0);
			if(Ydist>0)
			{
				posY  += 10;
				Ydist -= 10;
				//increment the posY for we moved the machine
				//decrease the value that we still have to go 
			}
			else
			{
				posY  -= 10;
				Ydist += 10;
			}
					
		}
		double XtimetoGo, YtimetoGo;                            
		if(Xdist>0)                                                                                     //0 1 2 3 4 5 6 7 8 9 11 12 13
													//	      |posX=2     |target=8 xdist=6
		{                   //becues timing[0] values get bigger as the index increase this is positive           
			XtimetoGo = (2)*Xdist*(timing[(posX)/10+1][0] - timing[posX/10][0]);
		}             //becaues we are runing at 50% power and becues it is a fraction of a full tick 
		else 
		{                   //becues timing[1] vlues get bigger as the index decrses this is positive
			XtimetoGo = (-2)*Xdist*(timing[(posX)/10-1][1]- timing[posX/10][1]);
		}
		if(Ydist>0)
		{
			YtimetoGo = (2)*Ydist*(timing[(posY)/10+1][2] - timing[posY/10][2]);
		}
		else 
		{
			YtimetoGo = (-2)*Ydist*(timing[(posY)/10-1][3]- timing[posY/10][3]);
		}
		XtimetoGo = XtimetoGo * (1000000000/CLOCKS_PER_SEC);//turn it to nano 
		YtimetoGo = YtimetoGo * (1000000000/CLOCKS_PER_SEC);//turn it to nano
		if(XtimetoGo<YtimetoGo) //do both at same time and then finish the y change
		{
			if(Xdist>0)
				power(motorXA,50);
			else
				power(motorXB,50);
			if(Ydist>0)
				power(motorYA,50);
			else 
				power(motorYB,50);

			nanosleep((const struct timespec[]){{0,XtimetoGo }}, NULL);
			power(motorXA,0);
			power(motorXB,0);
			nanosleep((const struct timespec[]){{0,YtimetoGo-XtimetoGo }}, NULL);
			power(motorYA,0);
			power(motorYB,0);
		}
		else
		{
			if(Xdist>0)
				power(motorXA,50);
			else
				power(motorXB,50);
			if(Ydist>0)
				power(motorYA,50);
			else 
				power(motorYB,50);

			nanosleep((const struct timespec[]){{0,YtimetoGo }}, NULL);
			power(motorYA,0);
			power(motorYB,0);
			nanosleep((const struct timespec[]){{0,XtimetoGo-YtimetoGo }}, NULL);
			power(motorXA,0);
			power(motorXB,0);
		}
		posX += Xdist;
		posY += Ydist;
		return;
		
	}
	//so now the pen is down and we need to move very percisly...
	/// lets do some calulations to see the rates we need to go inorder for reaching the end point at the exact right time
	double Xtime = 0.0;
	double Ytime = 0.0;
	if(Xdist>0) //going foward x wise
	{                                                                    //                                             |..............|
		Xtime += timing[(posX+Xdist)/10][0]-timing[posX/10][0];//gets the bigest chuck of the time slot this gets 0.0  x   1.0   2.0  target  3.0
						                                                                          //   0.3             2.8
		Xtime += (((double)((Xdist+posX)%10))/10)*(timing[(posX+Xdist)/10+1][0]-timing[(posX+Xdist)/10][0]); 
												// get the last part of the time interbol  |....| 
		Xtime -= (((double)(posX%10))/10)*(timing[posX/10+1][0] - timing[posX/10][0]);	//subtract the overshoot we had             |---| 
	}
	else if (Xdist < 0) //going backwords x wise
	{ 														//            |.....|
		Xtime += timing[(posX+Xdist)/10][1]-timing[posX/10][1];//gets the bigest chuck of the time slot this gets 0.0 target 1.0   2.0  x  3.0
		
		Xtime += (((double)(posX%10))/10)*(timing[posX/10][1]-timing[posX/10+1][1]); 	  // get the last part of the time interbol  |..| 
		
		Xtime -= (((double)((posX+Xdist)%10))/10)*(timing[(posX+Xdist)/10][1]-timing[(posX+Xdist)/10+1][1]);
				}
	else //we are within 10 of where we need to go. tread carefully
	{
		Xtime = 0.0;
	}
	if(Ydist>0) //going foward Y wise
	{                                                                    //                                             |..............|
		Ytime += timing[(posY+Ydist)/10][2]-timing[posY/10][2];//gets the bigest chuck of the time slot this gets 0.0  x   1.0   2.0  target  3.0
						                                                                          //   0.3             2.8
		Ytime += (((double)((Ydist+posY)%10))/10)*(timing[(posY+Ydist)/10+1][2]-timing[(posY+Ydist)/10][2]); 
												// get the last part of the time interbol  |....| 
		Ytime -= (((double)(posY%10))/10)*(timing[posY/10+1][2] - timing[posY/10][2]);	//subtract the overshoot we had             |---| 
	}
	else if (Ydist < 0) //going backwords Y wise
	{ 														//  |++++++++++++++++|
		Ytime += timing[(posY+Ydist)/10][3]-timing[posY/10][3];//gets the bigest chuck of the time slot this gets 0.0 target 1.0   2.0  x  3.0
		
		Ytime += (((double)(posY%10))/10)*(timing[posY/10][3]-timing[posY/10+1][3]); 	  // get the last part of the time interbol  |..| 
		
		Ytime -= (((double)((posY+Ydist)%10))/10)*(timing[(posY+Ydist)/10][3]-timing[(posY+Ydist)/10+1][3]);
													//this subtracts   |----|
	//redisgingting part
		//Ytime += timing[posY/10+Ydist/10][3]-timing[posY/10][3];//gets the bigest chuck of the time slot this gets 0.0 target 1.0   2.0  x  3.0
						                                                                          //   0.3             2.8
	//jj	Ytime += (((double)(posY%10))/10)*(timing[posY/10][3]-timing[posY/10+1][3]); 			  // get the last part of the time interbol  |..| 

	//	Ytime += (((double)(10-(Ydist+posY)%10))/10)*(timing[(Ydist+posY)/10][3]-timing[((Ydist+posY)/10)+1][3]);// get the first part  |.....| 
	}
	else //we are within 10 of where we need to go. tread carefully
	{
		Ytime = 0.0;
	}
}
void initalize()
{
	pinMode(Xstop,INPUT);
	pinMode(Ystop,INPUT);
	pinMode(motorXA,OUTPUT);
	pinMode(motorXB,OUTPUT);
	pinMode(motorYA,OUTPUT);
	pinMode(motorYB,OUTPUT);
	power(motorXA,0);
	power(motorXB,0);
	power(motorYA,0);
	power(motorYB,0);
	clearScreen();
	keypad(stdscr,TRUE);
	while(!quit)
	{
		mvprintw(4,10,"Initalizing ....");
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
	move(5,0);
	clrtoeol();
	mvprintw(5,10,"Finding position 0 on X axis...");
	refresh();
	if(digitalRead(Xstop))
	{
		power(motorXA,100);	
		safeDelay(500);
		power(motorXA,0);
	}
	clrtoeol();
	mvprintw(5,10,"Finding position 0 on Y axis...");
	refresh();
	power(motorXB,100);
	while(!quit&&!digitalRead(Xstop))
		safeDelay(1);
	posX=0;
	power(motorXB,0);
	if(digitalRead(Ystop))
	{
		power(motorYA,100);	
		safeDelay(500);
		power(motorYA,0);
	}
	power(motorYB,100);
	while(!quit&&!digitalRead(Ystop))
		safeDelay(1);
	posY=0;
	power(motorYB,0);
	clock_t before = clock();
	clrtoeol();
	mvprintw(5,10,"Seting up timing increment on the X axis...");
	refresh();
	power(motorXA,100);
	while(!quit&&posX/10<=MAXSIZEX)
	{
		if(didTick(1,0)==0)
			continue;
		timing[posX/10][0] = (long) clock();	
		posX+=10;
		safeDelay(1);
	}
	timing[(posX)/10+1][0] = 0;
	timing[(posX)/10+1][1] = 0;
	power(motorXA,0);
	printw("Going back down");
	power(motorXB,100);
	while(!quit&&posX/10>=0)
	{
		if(didTick(1,0)==0)
			continue;
		timing[posX/10][1] = (long) clock();	
		posX-=10;
		safeDelay(1);
	}
	clrtoeol();
	mvprintw(5,10,"Seting up timing increment on the Y axis...");
	refresh();
	power(motorXB,0);
	before = clock();
	power(motorYA,100);
	while(!quit&&posY/10<=MAXSIZEX)
	{
		if(didTick(1,0)==0)
			continue;
		timing[posY/10][2] = (long) clock();	
		posY+=10;
		safeDelay(1);
	}
	timing[(posY)/10+1][2] = 0;
	timing[(posY)/10+1][3] = 0;
	power(motorYA,0);
	power(motorYB,100);
	printw("Going back down");
	while(!quit&&posY/10>=0)
	{
		if(didTick(0,1)==0)
			continue;
		timing[posY/10][3] = (long) clock();	
		posY-=10;
		safeDelay(1);
	}
	power(motorYB,0);
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
	safeDelay(1000);
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
