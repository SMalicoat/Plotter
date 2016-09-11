//Project: Plotter/Printer/Milling machine
//Homepage: www.HomoFaciens.de
//Author Norbert Heinz
//Version: 0.1
//Creation date: 27.06.2012
//Copyright: (c) 2012 by Norbert Heinz
//This program is free software you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation version 3 of the License.
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with this program if not, see http://www.gnu.org/licenses/
//
//compile with gcc plotter.c -I/usr/local/include -L/usr/local/lib -lwiringPi -lm

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

#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <math.h>
#include <wiringPi.h>
#include <unistd.h>

#define WIDTH 30
#define HEIGHT 10 

#define DEBUG 1
#define STOPPULSE 150   //all of the pin numbers are wiringPI standard!!
#define optoSensorX1 4 
#define optoSensorX2 5
#define optoSensorY1 27 
#define optoSensorY2 28 
#define motorXA 7 
#define motorXB 0 
#define motorYA 2 
#define motorYB 3 
#define penServo 1   
#define Xstop 24 
#define Ystop 25      //the pin that the touch sensor is located on tellign the y asix to stop at 0

#define MAXSIZEX 100
#define MAXSIZEY 100






#define SERVOUP           50
#define SERVODOWN         1400

#define BUFFERSIZE         120


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
int safeDelay(double duration);
void allSTOP();
int didTick(int checkX,int checkY);
void power(int pin,int percent);




void sleepMs(long MilliSeconds);
int gotoxy(int x, int y);
void clrscr(int StartRow, int EndRow);
int kbhit(void);
int getch();
void MessageText(char *message, int x, int y, int alignment);
void PrintRow(char character, int y);
void ErrorText(char *message);
void PrintMenue_01(char * PlotFile, double scale, double width, double height, long MoveLength, int plotterMode);
char *PrintMenue_02(int StartRow, int selected, int plotterMode);
void PrintMenue_03(char *FullFileName, long NumberOfLines, long CurrentLine, long CurrentX, long CurrentY, long StartTime);
void MoveToSwitch(int X, int Y);
void SetDrops(int Cyan, int Magenta, int Yellow);
void MoveServo(int PulseWidth);
void MoveZ(long Pulses, long stepPause);
void MakeStepX(int direction, long stepPause);
void MakeStepY(int direction, long stepPause);
void MakeStepZ(int direction, long stepPause);
int CalculatePlotter(long moveX, long moveY, long stepPause);




int  MaxRows = 24;
int  MaxCols = 80;
int  MessageX = 1;
int  MessageY = 24;
int GPIO[17];


double penRateX = 0.9;
double penRateY = 0.9;
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
int main(int argc, char **argv){
{
int                 ADCvalue01 = 0;
int                 StepSpeed = 100;  //Stepping speed (smaler value = faster)
int                 i = 0;
int                 KeyHit = 0;
int                 KeyCode[5];
int                 SingleKey=0;
char                TextLine[300];
char                FileName[100] = "";
char                FileNameOld[100] = "";
char                FullFileName[300];
char                a;
char                IsDigit[]="-1234567890";
int                 ReadState = 0;
int                 MenueLevel = 0;
int                 FileSelected = 0;
int                 FileStartRow = 0;
int                 stopPlot = 0;
long                xMin = 1000000, xMax = -1000000;
long                yMin = 1000000, yMax = -1000000;
long                coordinateCount = 0;
long                coordinatePlot = 0;
long                currentPlotX = 0, currentPlotY = 0, currentPlotDown = 0;
long                xNow = 0, yNow = 0, zNow = 0;
char                *pEnd;
FILE                *PlotFile;
double              Scale = 1.0;
double              OldScale = 1.0;
double              PicWidth = 0.0;
double              PicHeight = 0.0;
long                MoveLength = 100;
long                OldMoveLength = 200;
struct timeval      StartTime, EndTime;
struct winsize      terminal;
long                PlotStartTime = 0;
int                 MoveFinished = 0;
long                stepPause = 5000;
long                stepPauseNormal =     5000;
long                stepPauseMaterial =  20000;
long                zHub = 200;
int                 plotterMode = 1;

char                FileInfo[3];
long                FileSize;
long                LongTemp;
long                DataOffset;
long                HeaderSize;
long                PictureWidth;
long                PictureHeight;
int                 IntTemp;
int                 ColorDepth;
long                CompressionType;
long                PictureSize;
long                XPixelPerMeter;
long                YPixelPerMeter;
long                ColorNumber;
long                ColorUsed;
int                 PixelRed, PixelGreen, PixelBlue;
int                 PixOK = 0;
long                StepsPerPixelX = 50, StepsPerPixelY = 50;
long                JetOffset1 = 40, JetOffset2 = 40;




  //Mapping between wiringPi and pin order of my I/O board.
  //This was necessary, because I used a wrong description of the GPIOs
  //You have to change the order according to your board!
  //@@@THESE ARE ALL OUTPUS!!
  

  strcpy(FileName, "noFiLE");

  if (wiringPiSetup () == -1){
    printf("Could not run wiringPiSetup!");
    exit(1);
  }

getmaxyx(stdscr, MaxRows, MaxCols);
MessageY = MaxRows-3;

  clrscr(1, MaxRows);
  PrintRow('-', MessageY - 1);
    initscr();
	noecho();
	cbreak();
	start_color();
	
	while(!quit)
		{
		clearScreen();
		char * choices[] = {"Plot File","Manual Control","Servo Control","Opto Sensor Control","X&Y Control","initalize","Exit"};
		char *description[] = {
		"You select a file to plot and will plot the file on the plotter",
		"Will give you full control over the plotter for debuging or demenstration",
		"Lets you enter and test different duration of pulses sent to the servo for fine tuning and debugging",
		"Will read in the togleing of the opto sensor for debuging",
		"Will let you have control how much to move in x and y by ticks more precisly",
		"Test initalization procedure",
		"Will exit the prgram"};
		int result = menus("Hello and Welcome to my creation please sleect a mode", choices,description,7,20);
		
		switch (result)
		{
			case 1:
			
			/*	FILE * fp;
				char * line = NULL; 
				size_t len = 0;
				ssize_t read;
				fp = fopen ("sampleGcode.txt","r");*/
				//Plot file
				
					char dir[1024];
					getcwd(dir,sizeof(dir));
					MessageText("3 seconds until plotting starts !!!!!!!!!!!!!!!!!", 1, 20, 0);
					delay(3000);
					if((PlotFile=chooseFile(dir))==NULL){
						sprintf(TextLine, "Can't open file '%s'!\n", FullFileName);
						strcpy(FileName, "NoFiLE");
						ErrorText(TextLine);
					}
					else{
					         
						currentPlotX = 0;
						currentPlotY = 0;        
						PlotStartTime = time(0);
						PrintMenue_03(FullFileName, coordinateCount, 0, 0, 0, PlotStartTime);
						coordinatePlot = 0;
						stopPlot = 0;
						if(currentPlotDown == 1){
						 
							penMove(1);
				
						 
						  currentPlotDown = 0;
						}
						
						while(!(feof(PlotFile)) && stopPlot == 0){
						  fread(&a, 1, 1, PlotFile);
						  if(a == '>'){
							ReadState = 0;
						  }
						  if(ReadState == 0 || ReadState == 1){
							TextLine[i] = a;
							TextLine[i+1] = '\0';
							i++;
						  }
						  if(ReadState == 2 || ReadState == 3){
							if(strchr(IsDigit, a) != NULL){
							  TextLine[i] = a;
							  TextLine[i+1] = '\0';
							  i++;
							}
						  }
						  if(ReadState == 2){
							if(strchr(IsDigit, a) == NULL){
							  if(strlen(TextLine) > 0){
								xNow = (strtol(TextLine, &pEnd, 10) - xMin) * 0.33333 * Scale;
								ReadState = 3;
								i=0;
								TextLine[0]='\0';
							  }
							}
						  }
						  if(ReadState == 3){
							if(strchr(IsDigit, a) == NULL){
							  if(strlen(TextLine) > 0){
								yNow = ((yMax - strtol(TextLine, &pEnd, 10)) - yMin) * 0.33333 * Scale;//Flip around y-axis
								coordinatePlot++;
								PrintMenue_03(FullFileName, coordinateCount, coordinatePlot, xNow, yNow, PlotStartTime);
								if(stopPlot == 0){
								  stopPlot =0;
								  moveXY(xNow - currentPlotX, yNow - currentPlotY);
								  currentPlotX = xNow;
								  currentPlotY = yNow;
			  //                    sprintf(TextLine, "xNow=%ld, yNow=%ld", xNow, yNow);
			  //                    MessageText(TextLine, MessageX+1, MessageY+1, 0);
			  //                    getch();
								}
								else{
								  if(currentPlotDown == 1){
									if(plotterMode == 1){
									  penMove(1)
									}
													   
									currentPlotDown = 0;
								  }
								}
								if(currentPlotDown == 0){
								  if(stopPlot == 0){
									
									  penMove(0);
									
									
									currentPlotDown = 1;
								  }
								}
								ReadState = 2;
								i=0;
								TextLine[0]='\0';
							  }
							}
						  }
						  if(strcmp(TextLine, "<path") == 0){
							if(currentPlotDown == 1){
							  	penMove(1);
							  
												   
							  currentPlotDown = 0;
							}
							ReadState = 2;
							i = 0;
							TextLine[0]='\0';
						  }
						  if(a == '<'){
							i = 1;
							TextLine[0] = a;
							TextLine[1] = '\0';
							ReadState = 1;
						  }
						}//while(!(feof(PlotFile)) && stopPlot == 0){
						fclose(PlotFile);
						if(currentPlotDown == 1){
						  
						   penMove(1);
						  
											  
						  currentPlotDown = 0;
						}
						PrintMenue_03(FullFileName, coordinateCount, coordinatePlot, 0, 0, PlotStartTime);
						moveXY(-currentPlotX, -currentPlotY);
						currentPlotX = 0;
						currentPlotY = 0;
						while(kbhit()){
						  getch();
						}
						MessageText("Finished! Press any key to return to main menu.", MessageX, MessageY, 0);
						getch();
						break;
						
					}
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
				initalize();
				break;
			
			case -1:
			default:
				system("reset");
				quit=true;
				break;

		}
	}
	allSTOP();
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
//			delay(1);
  			safeDelay(1);
			c=getch();
			if(result==0)
				continue;
			if(result==1)
				xticks=(isback)?xticks-1:xticks+1;
			if(result==-1)
				yticks=(isback)?yticks-1:yticks+1;
		
			move(14,0);
			clrtoeol();
			mvprintw(14,10,"\tX-Ticks:%d\tY-Ticks:%d\tXSTOP:%i",xticks,yticks,digitalRead(Xstop));
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
//		delay(10);
		safeDelay(10);
		refresh();
	}


	return 0;
}
void getMaxsizeofDIR(char * dir,int *files,int *length)

{
	DIR *dp;
	struct dirent *ep;
	*files = 0;
	*length = 0;

	dp = opendir (dir);
	if(dp != NULL)
	{

	while ((ep = readdir (dp))&&!quit)
	{
		
			if(ep->d_name[0]!='.'||ep->d_name[1]=='.')
			{
				++*files;
				int size = strlen(ep->d_name);
				*length = (*length > size)?*length:size;
			}
	}
		(void) closedir(dp);
	}
	else 
		perror("Could't open the directory");
}
FILE * chooseFile(char* dir)
{
	int files, length;
	getMaxsizeofDIR(dir,&files,&length);
	bool *isDIR = (bool *) malloc(files*sizeof(bool*));
	char **choices = (char**)malloc((files+3)*sizeof(char*));
	int i;
	for(i = 0; i < files; i++)
	{
		choices[i] = (char *)malloc(length*sizeof(char));

	}
	DIR *dp;
	struct dirent *ep;
	dp = opendir (dir);

	if(dp != NULL)
	{

		isDIR[0] = true;
		strcpy(choices[0],"\\..");
		i = 1;
		clearScreen();
		while(!quit)
		{

			if ((ep = readdir (dp))!=NULL)
			{
	
				if(ep->d_name[0]!='.')
				{
					isDIR[i] = ep->d_type==DT_DIR;

					if(isDIR[i])
					{
						strcpy(choices[i],"\\");
						strcat(choices[i],ep->d_name);
					}
					else
						strcpy(choices[i],ep->d_name);
					i++;
				}
			}
			else
			{

				break;
			}
		}
	int result = menus("Select file to be plotted",choices,NULL,files,length+3);
	if(result==-1)
		return NULL;
	char  fileName[1024];
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
		}
		last++;
		for(count = 0;count < last;count++)
		{
			fileName2[count] = fileName[count];
		}
		fileName2[last] = '\0';
		return chooseFile(fileName2);
	
	}
	int loop;
	char c = 'a';
	strcat(fileName,"\0");
	for(loop = 0;c !='\0';loop++)
	{
		 c = fileName[loop];
	}
	if(loop>2&&fileName[loop-2]!='/')	
		strcat(fileName,"/");

	if(isDIR[result-1])
	{

		strcat(fileName,&(choices[result-1][1]));
		return chooseFile(fileName);
	}
		strcat(fileName,choices[result-1]);
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


	
int safeDelay(double duration)
{
	mvprintw(22,0,"in safe Delay with duration of:%i",duration);
	if(quit)
		return 1;
	delay(duration);
	return 0;
	nodelay(stdscr,1);
	clock_t before = clock();
	//int c = getch();
	do
	{
		
		int newc = getch();
		printw("newc is:%i",newc);
		if(newc==27&&getch()==-1)
		{
			allSTOP();
			quit = true;
			return 1;
		}
		
		delay(1);
	}
	while(((double)((clock() - before )*(1000/CLOCKS_PER_SEC)))>duration);
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

	if(DEBUG)
		percent = percent/ 2;
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
void moveXY(int Xdist, int Ydist)
{
	char TextLine[1000] = "";
  long  tempX = 0, tempY = 0;
  int i = 0;
  unsigned char reverseX = 0, reverseY = 0;
  
  sprintf(TextLine, "Moving X: %ld, Moving Y: %ld", moveX, moveY);
  MessageText(TextLine, MessageX, MessageY, 0);

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
	if(Xdist==0&&Ydist==0)
		return;

	int Xspeed = 100;
	int Yspeed = 100;
	bool foundX = false;
	bool foundY = false;
	if(Xdist==0||Ydist==0)
	{
		
		if(Xdist>0)    //need to go fowards 
		{
			power(motorXA,Xspeed);
			power(motorXB,0);
		}else if (Xdist<0)  //need to go backword 
		{
			power(motorXA,0);
			power(motorXB,Xspeed);
		}else                //already at the correct x point
		{
			power(motorXA,0);
			power(motorXB,0);
			foundX = true;
		}
		if(Ydist>0)
		{                      //need to go fowards
			power(motorYA,Yspeed);
			power(motorYB,0);

		}else if (Ydist<0)  //need to go backwords
		{
			power(motorYA,0);
			power(motorYB,Yspeed);
		}else              //alrady at the correct pint
		{
			power(motorYA,0);
			power(motorYB,0);
			foundY = true;
		}
	}else
	{
		if(abs(Xdist)>=abs(Ydist))
		{
			Xspeed = 100; 
			Yspeed = (abs(Ydist)/abs(Xdist))*Xspeed;
		}
		else
		{
			Yspeed = 100; 
			Xspeed = (abs(Xdist)/abs(Ydist))*Yspeed;
		}
		
		
	}
		while(!foundX||!foundY)
		{
			int result = didTick(!foundX,!foundY);		
			
			if(!foundX)
			{
				if(result==1)
				{
					if(Xdist>0)
					{
						
						posX  += 10;
						Xdist -= 10;
						//increment the posY for we moved the machine
						//decrease the value that we still have to go 
					}
					else
					{
							
						posX  -= 10;
						Xdist += 10;
					}
					if(abs(Xdist)<20)
					{
						if(Xdist>0)    //need to go fowards 
						{
							power(motorXA,.5*Xspeed);
							power(motorXB,0);
						}else  //need to go backword 
						{
							power(motorXA,0);
							power(motorXB,.5*Xspeed);
						}
					
					}
					if(Xdist==0)
					{
						power(motorXA,0);
						power(motorXB,0);
						foundX = true;
					}
				}

			}
			if(!foundY)
			{
				if(result==-1)
				{
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
					if(abs(Ydist)<20)
					{
						if(Ydist>0)    //need to go fowards 
						{
							power(motorYA,.5*Yspeed);
							power(motorYB,0);
						}else  //need to go backword 
						{
							power(motorYA,0);
							power(motorYB,.5*Yspeed);
						}
					
					}
					if(Ydist==0)
					{
						power(motorYA,0);
						power(motorYB,0);
						foundY = true;
					}
				}
			}
			delay(1);
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
	bool stop = false;
	while(!quit&&!stop)
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
				stop = true;
				break;
			defaut:
				break;
		}
		
	}
	penMove(1);
	move(5,0);
	clrtoeol();
	nodelay(stdscr,0);

	mvprintw(5,10,"Finding position 0 on X axis...");
	getch();
	refresh();
	if(digitalRead(Xstop))
	{
		power(motorXA,100);	
		safeDelay(500);
		power(motorXA,0);
	}
	clrtoeol();
	mvprintw(5,10,"Finding position 0 on X axis...");
	getch();
	refresh();
	power(motorXB,50);
	while(!quit&&!digitalRead(Xstop))
		delay(3);

	posX=0;
	power(motorXB,0);
	printw("just found 0 on the x!");
	getch();
	
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
	allSTOP();
}
  char TextLine[1000] = "";
  long  tempX = 0, tempY = 0;
  int i = 0;
  unsigned char reverseX = 0, reverseY = 0;
  
  sprintf(TextLine, "Moving X: %ld, Moving Y: %ld", moveX, moveY);
  MessageText(TextLine, MessageX, MessageY, 0);

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
	if(Xdist==0&&Ydist==0)
		return;

	if(DEBUG||penUp)
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
					{
						power(motorXA,0);
						power(motorXB,0);
						foundX = true;
					}
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
	else //we are already there 
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
	else //we are alredy there 
	{
		Ytime = 0.0;
	}
	if(Ytime==0) //need to just move in the X direction
	{
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
	
		}	
	}

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
				power(motorXA,100);
				power(motorXB,0);
				delay(20);
				power(motorXA,0);
			//	movex(50);
				break;
			case KEY_DOWN:
				if(DEBUG)
				mvprintw(20,3,"Down arrrow pressed!");
				power(motorXB,100);
				power(motorXA,0);
				delay(20);
				power(motorXB,0);
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
				power(motorYB,100);
				power(motorYA,0);
				delay(20);
				power(motorYB,0);
				break;
			case KEY_RIGHT:
				if(DEBUG)
				mvprintw(20,3,"Right arrrow pressed!");
				power(motorYA,100);
				power(motorYB,0);
				delay(20);
				power(motorYA,0);
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
	refresh();
	clear();
	refresh();
	clearScreen();
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
	if(description!=NULL)
	{
	 descript_win = newwin(height, width+4, starty, startx+width+2);
	mvwprintw(descript_win, 2,2,"%s",description[highlight -1]);
	wrefresh(descript_win);
	}
	wrefresh(menu_win);
}




//+++++++++++++++++++++++ Start clrscr ++++++++++++++++++++++++++
//@@@@MODIFIED!!!!
void clrscr(int StartRow, int EndRow) {
  int i, i2;
  
  if (EndRow < StartRow){
    i = EndRow;
    EndRow = StartRow;
    StartRow = i;
  }
  for (i = StartRow; i <= EndRow; i++){
  	move(i,0);
	clrtoeol();
    
  }
}
//@@@MODIFIED
//----------------------- End clrscr ----------------------------

//+++++++++++++++++++++++ Start kbhit ++++++++++++++++++++++++++++++++++
//Thanks to Undertech Blog, http://www.undertec.de/blog/2009/05/kbhit_und_getch_fur_linux.html
int kbhit(void) {

   int c = 0;
   nodelay(stdscr,1);
   c = getch();
   if (c != -1)
	   ungetch(c);

   return ((c != -1) ? 1 : 0);

}
//------------------------ End kbhit -----------------------------------

//++++++++++++++++++++++ Start MessageText +++++++++++++++++++++++++++++
void MessageText(char *message, int x, int y, int alignment){
  int i;
  char TextLine[300];

  clrscr(y, y);
  move(x, y);
  
  TextLine[0] = '\0';
  if(alignment == 1){
    for(i=0; i < (MaxCols - strlen(message)) / 2 ; i++){
      strcat(TextLine, " ");
    }
  }
  strcat(TextLine, message);
  
  printw("%s\n", TextLine);
}
//-------------------------- End MessageText ---------------------------

//++++++++++++++++++++++ Start PrintRow ++++++++++++++++++++++++++++++++
void PrintRow(char character, int y){
  int i;
  move(y,0);
  for(i=0; i<MaxCols;i++){
    printw("%c", character);
  }
}
//-------------------------- End PrintRow ------------------------------

//+++++++++++++++++++++++++ ErrorText +++++++++++++++++++++++++++++
void ErrorText(char *message){
  clrscr(MessageY + 2, MessageY + 2);
  gotoxy (1, MessageY + 2);  
  printf("Last error: %s", message);
}
//----------------------------- ErrorText ---------------------------

//+++++++++++++++++++++++++ PrintMenue_01 ++++++++++++++++++++++++++++++
void PrintMenue_01(char * PlotFile, double scale, double width, double height, long MoveLength, int plotterMode){
  char TextLine[300];
  
   clrscr(1, MessageY-2);
   MessageText("*** Main menu plotter ***", 1, 1, 1);
   sprintf(TextLine, "M            - move length, current value = %ld steps. F5=1, F6=10, F7=100, F8=1000", MoveLength);
   MessageText(TextLine, 10, 3, 0);
   MessageText("Cursor right - move plotter in positive X direction", 10, 4, 0);
   MessageText("Cursor left  - move plotter in negative X direction", 10, 5, 0);
   MessageText("Cursor up    - move plotter in positive Y direction", 10, 6, 0);
   MessageText("Cursor down  - move plotter in negative Y direction", 10, 7, 0);
   if(plotterMode == 1){
     MessageText("Page up      - lift pen", 10, 8, 0);
     MessageText("Page down    - touch down pen", 10, 9, 0);
   }
   if(plotterMode == 0){
     MessageText("Page up      - move Plotter in positive Z direction", 10, 8, 0);
     MessageText("Page down    - move Plotter in negative Z direction", 10, 9, 0);
   }
   sprintf(TextLine, "F            - choose file. Current file = \"%s\"", PlotFile);
   MessageText(TextLine, 10, 10, 0);
   MessageText("0            - move plotter to 0/0", 10, 11, 0);
   sprintf(TextLine, "S            - scale. Current value = %0.4f. W = %0.1fcm, H = %0.1fcm", scale, width, height);
   MessageText(TextLine, 10, 12, 0);
   MessageText("P            - plot file", 10, 13, 0);
   if(plotterMode == 0){
     MessageText("T            - toggle functionality, currently: MILLING", 10, 14, 0);
   }
   if(plotterMode == 1){
     MessageText("T            - toggle functionality, currently: PLOTTING", 10, 14, 0);
   }
   if(plotterMode == 2){
     MessageText("T            - toggle functionality, currently: PRINTING", 10, 14, 0);
   }

   MessageText("Esc          - leave program", 10, 16, 0);
   
}
//------------------------- PrintMenue_01 ------------------------------

//+++++++++++++++++++++++++ PrintMenue_02 ++++++++++++++++++++++++++++++
char *PrintMenue_02(int StartRow, int selected, int plotterMode){
  char TextLine[300];
  char OpenDirName[1000];
  static char FileName[101];
  char tempChar[100];
  DIR *pDIR;
  struct dirent *pDirEnt;
  int i = 0;  
  int Discard = 0;
  
  clrscr(1, MessageY-2);
  MessageText("*** Choose plotter file ***", 1, 1, 1);
   
  strcpy(OpenDirName, PICTUREPATH);
  

  pDIR = opendir(OpenDirName);
  if ( pDIR == NULL ) {
    sprintf(TextLine, "Could not open directory '%s'!", OpenDirName);
    MessageText(TextLine, 1, 4, 1);
    getch();
    return( "" );
  }

  pDirEnt = readdir( pDIR );
  while ( pDirEnt != NULL && i < 10) {
    strcpy(tempChar, pDirEnt->d_name);
    if(strlen(pDirEnt->d_name)>4 && ((tempChar[strlen(tempChar)-1] == 'p' && plotterMode == 2) || (tempChar[strlen(tempChar)-1] == 'g' && plotterMode < 2))){
      if(Discard >= StartRow){
        if(i + StartRow == selected){
          sprintf(TextLine, ">%s<", pDirEnt->d_name);
          strcpy(FileName, pDirEnt->d_name);
        }
        else{
          sprintf(TextLine, " %s ", pDirEnt->d_name); 
        }
        MessageText(TextLine, 1, 3 + i, 0);
        i++;
      }
      Discard++;

    }
    pDirEnt = readdir( pDIR );
  }  

  gotoxy(MessageX, MessageY + 1);
  printf("Choose file using up/down keys and confirm with 'Enter' or press 'Esc' to cancel.");
  

  return (FileName);
}
//------------------------- PrintMenue_02 ------------------------------


//+++++++++++++++++++++++++ PrintMenue_03 ++++++++++++++++++++++++++++++
void PrintMenue_03(char *FullFileName, long NumberOfLines, long CurrentLine, long CurrentX, long CurrentY, long StartTime){
  char TextLine[300];
  long CurrentTime, ProcessHours = 0, ProcessMinutes = 0, ProcessSeconds = 0;
  
   CurrentTime = time(0);
   
   CurrentTime -= StartTime;
   
   while (CurrentTime > 3600){
     ProcessHours++;
     CurrentTime -= 3600;
   }
   while (CurrentTime > 60){
     ProcessMinutes++;
     CurrentTime -= 60;
   }
   ProcessSeconds = CurrentTime;
   
   clrscr(1, MessageY - 2);
   MessageText("*** Plotting file ***", 1, 1, 1);
   
   sprintf(TextLine, "File name: %s", FullFileName);
   MessageText(TextLine, 10, 3, 0);
   sprintf(TextLine, "Number of lines: %ld", NumberOfLines);
   MessageText(TextLine, 10, 4, 0);
   sprintf(TextLine, "Current Position(%ld): X = %ld, Y = %ld     ", CurrentLine, CurrentX, CurrentY);
   MessageText(TextLine, 10, 5, 0);
   sprintf(TextLine, "Process time: %02ld:%02ld:%02ld", ProcessHours, ProcessMinutes, ProcessSeconds);
   MessageText(TextLine, 10, 6, 0);
     

}
//------------------------- PrintMenue_03 ------------------------------






