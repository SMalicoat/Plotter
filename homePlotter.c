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


#define PICTUREPATH         "/home/pi/pictures"

#define SERVOUP           50
#define SERVODOWN         1400

#define BUFFERSIZE         120

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


//++++++++++++++++++++++++++++++++++++++ sleepMs ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void sleepMs(long MilliSeconds){
  struct timeval      StartTime, EndTime;


  gettimeofday(&StartTime, NULL);
  gettimeofday(&EndTime, NULL);
  while((EndTime.tv_sec  - StartTime.tv_sec) * 1000 + (EndTime.tv_usec - StartTime.tv_usec) / 1000 < MilliSeconds){
    gettimeofday(&EndTime, NULL);
  }

}
//-------------------------------------- sleepMs ------------------------------------------------------------


//+++++++++++++++++++++++ Start gotoxy ++++++++++++++++++++++++++
//Thanks to 'Stack Overflow', found on http://www.daniweb.com/software-development/c/code/216326
int gotoxy(int x, int y) {
  char essq[100]; // String variable to hold the escape sequence
  char xstr[100]; // Strings to hold the x and y coordinates
  char ystr[100]; // Escape sequences must be built with characters
   
  //Convert the screen coordinates to strings.
  sprintf(xstr, "%d", x);
  sprintf(ystr, "%d", y);
   
  //Build the escape sequence (vertical move).
  essq[0] = '\0';
  strcat(essq, "\033[");
  strcat(essq, ystr);
   
  //Described in man terminfo as vpa=\E[%p1%dd. Vertical position absolute.
  strcat(essq, "d");
   
  //Horizontal move. Horizontal position absolute
  strcat(essq, "\033[");
  strcat(essq, xstr);
  // Described in man terminfo as hpa=\E[%p1%dG
  strcat(essq, "G");
   
  //Execute the escape sequence. This will move the cursor to x, y
  printf("%s", essq);
  return 0;
}
//------------------------ End gotoxy ----------------------------------

//+++++++++++++++++++++++ Start clrscr ++++++++++++++++++++++++++
void clrscr(int StartRow, int EndRow) {
  int i, i2;
  
  if (EndRow < StartRow){
    i = EndRow;
    EndRow = StartRow;
    StartRow = i;
  }
  gotoxy(1, StartRow);
  for (i = 0; i <= EndRow - StartRow; i++){
    for(i2 = 0; i2 < MaxCols; i2++){
      printf(" ");
    }
    printf("\n");
  }
}
//----------------------- End clrscr ----------------------------

//+++++++++++++++++++++++ Start kbhit ++++++++++++++++++++++++++++++++++
//Thanks to Undertech Blog, http://www.undertec.de/blog/2009/05/kbhit_und_getch_fur_linux.html
int kbhit(void) {

   struct termios term, oterm;
   int fd = 0;
   int c = 0;
   
   tcgetattr(fd, &oterm);
   memcpy(&term, &oterm, sizeof(term));
   term.c_lflag = term.c_lflag & (!ICANON);
   term.c_cc[VMIN] = 0;
   term.c_cc[VTIME] = 1;
   tcsetattr(fd, TCSANOW, &term);
   c = getchar();
   tcsetattr(fd, TCSANOW, &oterm);
   if (c != -1)
   ungetc(c, stdin);

   return ((c != -1) ? 1 : 0);

}
//------------------------ End kbhit -----------------------------------

//+++++++++++++++++++++++ Start getch ++++++++++++++++++++++++++++++++++
//Thanks to Undertech Blog, http://www.undertec.de/blog/2009/05/kbhit_und_getch_fur_linux.html
int getch(){
   static int ch = -1, fd = 0;
   struct termios new, old;

   fd = fileno(stdin);
   tcgetattr(fd, &old);
   new = old;
   new.c_lflag &= ~(ICANON|ECHO);
   tcsetattr(fd, TCSANOW, &new);
   ch = getchar();
   tcsetattr(fd, TCSANOW, &old);

//   printf("ch=%d ", ch);

   return ch;
}
//------------------------ End getch -----------------------------------

//++++++++++++++++++++++ Start MessageText +++++++++++++++++++++++++++++
void MessageText(char *message, int x, int y, int alignment){
  int i;
  char TextLine[300];

  clrscr(y, y);
  gotoxy (x, y);
  
  TextLine[0] = '\0';
  if(alignment == 1){
    for(i=0; i < (MaxCols - strlen(message)) / 2 ; i++){
      strcat(TextLine, " ");
    }
  }
  strcat(TextLine, message);
  
  printf("%s\n", TextLine);
}
//-------------------------- End MessageText ---------------------------

//++++++++++++++++++++++ Start PrintRow ++++++++++++++++++++++++++++++++
void PrintRow(char character, int y){
  int i;
  gotoxy (1, y);
  for(i=0; i<MaxCols;i++){
    printf("%c", character);
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


//++++++++++++++++++++++++++++++ MoveToSwitch +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MoveToSwitch(int X, int Y){

  if(X == 1){
    printf("Moving to X Switch is not defined in source code!!!");
  }

}


//++++++++++++++++++++++++++++++ SetDrops ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void SetDrops(int Cyan, int Magenta, int Yellow){
  int maxDrops=0;
  int i=0;
  
  maxDrops = Cyan;
  if(Magenta > maxDrops){
    maxDrops = Magenta;
  }
  if(Yellow > maxDrops){
    maxDrops = Yellow;
  }

  for(i = 0; i < maxDrops; i++){
    if(Cyan > 0){
      digitalWrite (GPIO[8], 1);
      Cyan--;
    }
    if(Magenta > 0){
      digitalWrite (GPIO[9], 1);
      Magenta--;
    }
    if(Yellow > 0){
      digitalWrite (GPIO[10], 1);
      Yellow--;
    }
    usleep(200000);
    digitalWrite (GPIO[8], 0);
    digitalWrite (GPIO[9], 0);
    digitalWrite (GPIO[10], 0);
    usleep(20000);
  }
}

//++++++++++++++++++++++++++++++ MoveServo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MoveServo(int PulseWidth){
struct timeval  StartTime, EndTime;

  PulseWidth += 1000;
  gettimeofday(&StartTime, NULL);
  gettimeofday(&EndTime, NULL);
  while((EndTime.tv_sec  - StartTime.tv_sec) * 1000 + (EndTime.tv_usec - StartTime.tv_usec) / 1000 < 1000){
    digitalWrite (GPIO[13], 1);
    usleep(PulseWidth);
    digitalWrite (GPIO[13], 0);    
    usleep(20000 - (PulseWidth));
    gettimeofday(&EndTime, NULL);
  }

}

//++++++++++++++++++++++++++++++ MoveZ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MoveZ(long Pulses, long stepPause){
  static int stepZ = 0;
  long i;

  if(Pulses > 0){
    for(i = 0; i < Pulses; i++){
      stepZ++;
      if(stepZ == 4){
        stepZ = 0;
      } 
      if(stepZ == 0){
        digitalWrite (GPIO[8], 1);
        digitalWrite (GPIO[9], 0);
        digitalWrite (GPIO[10], 0);
        digitalWrite (GPIO[11], 0);
      }
      if(stepZ == 1){
        digitalWrite (GPIO[8], 0);
        digitalWrite (GPIO[9], 0);
        digitalWrite (GPIO[10], 1);
        digitalWrite (GPIO[11], 0);
      }
      if(stepZ == 2){
        digitalWrite (GPIO[8], 0);
        digitalWrite (GPIO[9], 1);
        digitalWrite (GPIO[10], 0);
        digitalWrite (GPIO[11], 0);
      }
      if(stepZ == 3){
        digitalWrite (GPIO[8], 0);
        digitalWrite (GPIO[9], 0);
        digitalWrite (GPIO[10], 0);
        digitalWrite (GPIO[11], 1);
      }
      usleep(stepPause);
    }
  }

  if(Pulses < 0){
    Pulses = -Pulses;
    for(i = 0; i < Pulses; i++){
      stepZ--;
      if(stepZ == -1){
        stepZ = 3;
      } 
      if(stepZ == 0){
        digitalWrite (GPIO[8], 1);
        digitalWrite (GPIO[9], 0);
        digitalWrite (GPIO[10], 0);
        digitalWrite (GPIO[11], 0);
      }
      if(stepZ == 1){
        digitalWrite (GPIO[8], 0);
        digitalWrite (GPIO[9], 0);
        digitalWrite (GPIO[10], 1);
        digitalWrite (GPIO[11], 0);
      }
      if(stepZ == 2){
        digitalWrite (GPIO[8], 0);
        digitalWrite (GPIO[9], 1);
        digitalWrite (GPIO[10], 0);
        digitalWrite (GPIO[11], 0);
      }
      if(stepZ == 3){
        digitalWrite (GPIO[8], 0);
        digitalWrite (GPIO[9], 0);
        digitalWrite (GPIO[10], 0);
        digitalWrite (GPIO[11], 1);
      }
      usleep(stepPause);
    }
  }

}

//++++++++++++++++++++++++++++++ MakeStepX ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MakeStepX(int direction, long stepPause){
  static int stepX = 0;
  
  if(direction == 1){
    stepX++;
    if(stepX == 4){
      stepX = 0;
    }
  }

  if(direction == -1){
    stepX--;
    if(stepX == -1){
      stepX = 3;
    }
  }
  
  if(stepX == 0){
    digitalWrite (GPIO[0], 1);
    digitalWrite (GPIO[1], 0);
    digitalWrite (GPIO[2], 0);
    digitalWrite (GPIO[3], 0);
  }
  if(stepX == 1){
    digitalWrite (GPIO[0], 0);
    digitalWrite (GPIO[1], 0);
    digitalWrite (GPIO[2], 1);
    digitalWrite (GPIO[3], 0);
  }
  if(stepX == 2){
    digitalWrite (GPIO[0], 0);
    digitalWrite (GPIO[1], 1);
    digitalWrite (GPIO[2], 0);
    digitalWrite (GPIO[3], 0);
  }
  if(stepX == 3){
    digitalWrite (GPIO[0], 0);
    digitalWrite (GPIO[1], 0);
    digitalWrite (GPIO[2], 0);
    digitalWrite (GPIO[3], 1);
  }
  usleep(stepPause);
}

//++++++++++++++++++++++++++++++ MakeStepY ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MakeStepY(int direction, long stepPause){
  static int stepY = 0;
  
  if(direction == 1){
    stepY++;
    if(stepY == 4){
      stepY = 0;
    }
  }

  if(direction == -1){
    stepY--;
    if(stepY == -1){
      stepY = 3;
    }
  }
  
  if(stepY == 0){
    digitalWrite (GPIO[4], 1);
    digitalWrite (GPIO[5], 0);
    digitalWrite (GPIO[6], 0);
    digitalWrite (GPIO[7], 0);
  }
  if(stepY == 1){
    digitalWrite (GPIO[4], 0);
    digitalWrite (GPIO[5], 0);
    digitalWrite (GPIO[6], 1);
    digitalWrite (GPIO[7], 0);
  }
  if(stepY == 2){
    digitalWrite (GPIO[4], 0);
    digitalWrite (GPIO[5], 1);
    digitalWrite (GPIO[6], 0);
    digitalWrite (GPIO[7], 0);
  }
  if(stepY == 3){
    digitalWrite (GPIO[4], 0);
    digitalWrite (GPIO[5], 0);
    digitalWrite (GPIO[6], 0);
    digitalWrite (GPIO[7], 1);
  }
  usleep(stepPause);
}

//++++++++++++++++++++++++++++++ MakeStepZ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MakeStepZ(int direction, long stepPause){
  static int stepZ = 0;
  
  if(direction == 1){
    stepZ++;
    if(stepZ == 4){
      stepZ = 0;
    }
  }

  if(direction == -1){
    stepZ--;
    if(stepZ == -1){
      stepZ = 3;
    }
  }
  
  if(stepZ == 0){
    digitalWrite (GPIO[8], 1);
    digitalWrite (GPIO[9], 0);
    digitalWrite (GPIO[10], 0);
    digitalWrite (GPIO[11], 0);
  }
  if(stepZ == 1){
    digitalWrite (GPIO[8], 0);
    digitalWrite (GPIO[9], 0);
    digitalWrite (GPIO[10], 1);
    digitalWrite (GPIO[11], 0);
  }
  if(stepZ == 2){
    digitalWrite (GPIO[8], 0);
    digitalWrite (GPIO[9], 1);
    digitalWrite (GPIO[10], 0);
    digitalWrite (GPIO[11], 0);
  }
  if(stepZ == 3){
    digitalWrite (GPIO[8], 0);
    digitalWrite (GPIO[9], 0);
    digitalWrite (GPIO[10], 0);
    digitalWrite (GPIO[11], 1);
  }
  usleep(stepPause);
}


//++++++++++++++++++++++++++++++++++++++ CalculatePlotter ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CalculatePlotter(long moveX, long moveY, long stepPause){
  char TextLine[1000] = "";
  long  tempX = 0, tempY = 0;
  int i = 0;
  unsigned char reverseX = 0, reverseY = 0;
  
  sprintf(TextLine, "Moving X: %ld, Moving Y: %ld", moveX, moveY);
  MessageText(TextLine, MessageX, MessageY, 0);
//  getch();



  if(moveX == 0){
    if(moveY > 0){
      for(i = 0; i < moveY; i++){
         MakeStepY(1, stepPause);
      }
    }
    if(moveY < 0){
      for(i = 0; i < -moveY; i++){
         MakeStepY(-1, stepPause);
      }
    }
  }
  if(moveY == 0){
    if(moveX > 0){
      for(i = 0; i < moveX; i++){
         MakeStepX(1, stepPause);
      }
    }
    if(moveX < 0){
      for(i = 0; i < -moveX; i++){
         MakeStepX(-1, stepPause);
      }
    }
  }
  if(moveY != 0 && moveX != 0){
    if(abs(moveX) > abs(moveY)){
      while(moveY != 0){
        tempX = moveX / abs(moveY);
        if(tempX == 0){
          printf("tempX=%ld, moveX=%ld, moveY=%ld    \n", tempX, moveX, moveY);
        }
        if(tempX > 0){
          for(i = 0; i < tempX; i++){
             MakeStepX(1, stepPause);
          }
        }
        if(tempX < 0){
          for(i = 0; i < -tempX; i++){
             MakeStepX(-1, stepPause);
          }  
        }
        moveX -= tempX;
        if(moveY > 0){
          MakeStepY(1, stepPause);
          moveY--;
        }
        if(moveY < 0){
          MakeStepY(-1, stepPause);
          moveY++;
        }
      }
      //move remaining X coordinates
      if(moveX > 0){
        for(i = 0; i < moveX; i++){
           MakeStepX(1, stepPause);
        }
      }
      if(moveX < 0){
        for(i = 0; i < -moveX; i++){
           MakeStepX(-1, stepPause);
        }  
      }
    }//if(abs(moveX) > abs(moveY))
    else{
      while(moveX != 0){
        tempY = moveY / abs(moveX);
        if(tempY == 0){
          printf("tempY=%ld, moveX=%ld, moveY=%ld    \n", tempX, moveX, moveY);
        }
        if(tempY > 0){
          for(i = 0; i < tempY; i++){
             MakeStepY(1, stepPause);
          }
        }
        if(tempY < 0){
          for(i = 0; i < -tempY; i++){
             MakeStepY(-1, stepPause);
          }  
        }
        moveY -= tempY;
        if(moveX > 0){
          MakeStepX(1, stepPause);
          moveX--;
        }
        if(moveX < 0){
          MakeStepX(-1, stepPause);
          moveX++;
        }
      }
      //move remaining Y coordinates
      if(moveY > 0){
        for(i = 0; i < moveY; i++){
           MakeStepY(1, stepPause);
        }
      }
      if(moveY < 0){
        for(i = 0; i < -moveY; i++){
           MakeStepY(-1, stepPause);
        }  
      }
    }
  }

  return 0; 
}
//-------------------------------------- CalculatePlotter --------------------------------------------------------


int main(int argc, char **argv){

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
long                CyanDrops, MagentaDrops, YellowDrops;
int                 ReverseMode, NewLine;

FileInfo[2]='\0';

  //Mapping between wiringPi and pin order of my I/O board.
  //This was necessary, because I used a wrong description of the GPIOs
  //You have to change the order according to your board!
  GPIO[0]=8;
  GPIO[1]=9;
  GPIO[2]=7;
  GPIO[3]=10;
  GPIO[4]=11;
  GPIO[5]=12;
  GPIO[6]=13;
  GPIO[7]=14;
  GPIO[8]=15;
  GPIO[9]=16;
  GPIO[10]=0;
  GPIO[11]=1;
  GPIO[12]=2;
  GPIO[13]=3;
  GPIO[14]=4;
  GPIO[15]=5;
  GPIO[16]=6;

  strcpy(FileName, "noFiLE");

  if (wiringPiSetup () == -1){
    printf("Could not run wiringPiSetup!");
    exit(1);
  }

  for(i=0; i<17; i++){
    pinMode (GPIO[i], OUTPUT);
  }

  if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal)<0){
    printf("Can't get size of terminal window");
  }
  else{
    MaxRows = terminal.ws_row;
    MaxCols = terminal.ws_col;
    MessageY = MaxRows-3;
  }

  clrscr(1, MaxRows);
  PrintRow('-', MessageY - 1);
  PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);

 
  while (1){
    MessageText("Waiting for key press.", MessageX, MessageY, 0);

    i = 0;
    SingleKey = 1;
    KeyCode[0] = 0;
    KeyCode[1] = 0;
    KeyCode[2] = 0;
    KeyCode[3] = 0;
    KeyCode[4] = 0;
    KeyHit = 0;
    while (kbhit()){
      KeyHit = getch();
      KeyCode[i] = KeyHit;
      i++;
      if(i == 5){
        i = 0;
      }
      if(i > 1){
        SingleKey = 0;
      }
    }
    if(SingleKey == 0){
      KeyHit = 0;
    }

    if(MenueLevel == 0){
    
      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 68 && KeyCode[3] == 0 && KeyCode[4] == 0){
        CalculatePlotter(-MoveLength, 0, stepPause);
      }

      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 67 && KeyCode[3] == 0 && KeyCode[4] == 0){
        CalculatePlotter(MoveLength, 0, stepPause);
      }

      
      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 65 && KeyCode[3] == 0 && KeyCode[4] == 0){
        CalculatePlotter(0, MoveLength, stepPause);
      }

      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 66 && KeyCode[3] == 0 && KeyCode[4] == 0){
        CalculatePlotter(0, -MoveLength, stepPause);
      }


      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 53 && KeyCode[3] == 126 && KeyCode[4] == 0){
        if(plotterMode == 1){
          MoveServo(SERVOUP);
        }
        if(plotterMode == 0){
          MoveZ(MoveLength, stepPause);
        }
        currentPlotDown = 1;
      }

      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 54 && KeyCode[3] == 126 && KeyCode[4] == 0){
        if(plotterMode == 1){
          MoveServo(SERVODOWN);
        }
        if(plotterMode == 0){
          MoveZ(-MoveLength, stepPause);
        }
        if(plotterMode == 2){
          SetDrops(1, 1, 1);
        }
        currentPlotDown = 1;
      }

      if(KeyHit == 's'){//Ask for new scale value
        if(plotterMode == 0 || plotterMode == 1){
          OldScale = Scale;
          MessageText("Type new scale value: ", 1, MessageY, 0);
          gotoxy(23, MessageY);
          scanf("%lf", &Scale);
          if(Scale == 0){
            Scale = OldScale;
          }
          else{
            if(strlen(FileName) == 0){
              PicWidth = 0.0;
              PicHeight = 0.0;
            }
            else{
              PicWidth = (double)(xMax - xMin) * Scale / 1000.0;
              PicHeight = (double)(yMax - yMin) * Scale / 1000.0;    
            }
            PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
          }
        }//if(plotterMode == 0 || plotterMode == 1){
        else{
          ErrorText("Scale not supported in PRINTER mode!");
        }
      }

      if(KeyHit == 't'){//Toggle betwenn functionality (Plotting/Printing/Milling)
        plotterMode++;
        if(plotterMode == 3){
          plotterMode = 0;
        }
        if(plotterMode == 0 || plotterMode == 2){
          strcpy(FileName, "noFiLE");
          Scale = 1.0;
        }
        PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
      }

      if(KeyHit == 'm'){//Length of movement per key press
        OldMoveLength = MoveLength;
        MessageText("Type step length: ", 1, MessageY, 0);
        gotoxy(20, MessageY);
        scanf("%ld", &MoveLength);
        if(MoveLength == 0){
          MoveLength = OldMoveLength;
        }
        else{
          PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
        }
      }

      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 49 && KeyCode[3] == 53 && KeyCode[4] == 126){// F5 button
        MoveLength = 1;
        PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
      }
      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 49 && KeyCode[3] == 55 && KeyCode[4] == 126){// F6 button
        MoveLength = 10;
        PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
      }
      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 49 && KeyCode[3] == 56 && KeyCode[4] == 126){// F7 button
        MoveLength = 100;
        PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
      }
      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 49 && KeyCode[3] == 57 && KeyCode[4] == 126){// F8 button
        MoveLength = 1000;
        PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
      }

//      if(KeyHit == '0'){//Not implemented in hardware yet
//        MessageText("Move X and Y axis to switch position (y/n)?", MessageX, MessageY, 0);
//        if(getch() == 'y'){
//          MessageText("Moving X and Y axis to switch position. Press any key to cancel movement.", MessageX, MessageY, 0);
//          MoveToSwitch(1,1);          
//        }
//      }
      
      if(KeyHit == 'f'){//Chose input file
        FileStartRow = 0;
        FileSelected = 0;
        strcpy(FileNameOld, FileName);
        strcpy(FileName, PrintMenue_02(FileStartRow, 0, plotterMode));
        MenueLevel = 1;
      }
      
      if(KeyHit == 'p'){//Plot file
        MessageText("3 seconds until plotting starts !!!!!!!!!!!!!!!!!", 1, 20, 0);
        sleepMs(3000);
        if(strcmp(FileName, "noFiLE") != 0){
          if((PlotFile=fopen(FullFileName,"rb"))==NULL){
            sprintf(TextLine, "Can't open file '%s'!\n", FullFileName);
            strcpy(FileName, "NoFiLE");
            ErrorText(TextLine);
          }
        }
        if(strcmp(FileName, "noFiLE") != 0){
          if(plotterMode == 0 || plotterMode == 1){        
            currentPlotX = 0;
            currentPlotY = 0;        
            PlotStartTime = time(0);
            PrintMenue_03(FullFileName, coordinateCount, 0, 0, 0, PlotStartTime);
            coordinatePlot = 0;
            stopPlot = 0;
            if(currentPlotDown == 1){
              if(plotterMode == 1){
                MoveServo(SERVOUP);
              }
              if(plotterMode == 0){
                stepPause = stepPauseNormal;
                MoveZ(zHub, stepPause);
              }
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
                      stopPlot = CalculatePlotter(xNow - currentPlotX, yNow - currentPlotY, stepPause);
                      currentPlotX = xNow;
                      currentPlotY = yNow;
  //                    sprintf(TextLine, "xNow=%ld, yNow=%ld", xNow, yNow);
  //                    MessageText(TextLine, MessageX+1, MessageY+1, 0);
  //                    getch();
                    }
                    else{
                      if(currentPlotDown == 1){
                        if(plotterMode == 1){
                          MoveServo(SERVOUP);
                        }
                        if(plotterMode == 0){
                          stepPause = stepPauseNormal;
                          MoveZ(zHub, stepPause);
                        }                      
                        currentPlotDown = 0;
                      }
                    }
                    if(currentPlotDown == 0){
                      if(stopPlot == 0){
                        if(plotterMode == 1){
                          MoveServo(SERVODOWN);
                        }
                        if(plotterMode == 0){
                          stepPause = stepPauseMaterial;
                          MoveZ(-zHub, stepPause);
                        }
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
                  if(plotterMode == 1){
                    MoveServo(SERVOUP);
                  }
                  if(plotterMode == 0){
                    stepPause = stepPauseNormal;
                    MoveZ(zHub, stepPause);
                  }                      
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
              if(plotterMode == 1){
                MoveServo(SERVOUP);
              }
              if(plotterMode == 0){
                stepPause = stepPauseNormal;
                MoveZ(zHub, stepPause);
              }                      
              currentPlotDown = 0;
            }
            PrintMenue_03(FullFileName, coordinateCount, coordinatePlot, 0, 0, PlotStartTime);
            CalculatePlotter( -currentPlotX, -currentPlotY, stepPause );
            currentPlotX = 0;
            currentPlotY = 0;
            while(kbhit()){
              getch();
            }
            MessageText("Finished! Press any key to return to main menu.", MessageX, MessageY, 0);
            getch();
            PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
          }//if(plotterMode == 0 || plotterMode == 1){
          if(plotterMode == 2){//bitmap
            fread(&FileInfo, 2, 1, PlotFile);
            fread(&FileSize, 4, 1, PlotFile);
            fread(&LongTemp, 4, 1, PlotFile);
            fread(&DataOffset, 4, 1, PlotFile);
            fread(&HeaderSize, 4, 1, PlotFile);
            fread(&PictureWidth, 4, 1, PlotFile);
            fread(&PictureHeight, 4, 1, PlotFile);
            fread(&IntTemp, 2, 1, PlotFile);
            fread(&ColorDepth, 2, 1, PlotFile);
            fread(&CompressionType, 4, 1, PlotFile);
            fread(&PictureSize, 4, 1, PlotFile);
            fread(&XPixelPerMeter, 4, 1, PlotFile);
            fread(&YPixelPerMeter, 4, 1, PlotFile);
            fread(&ColorNumber, 4, 1, PlotFile);
            fread(&ColorUsed, 4, 1, PlotFile);
            
            fseek(PlotFile, DataOffset, SEEK_SET);
            ReverseMode = 0;
            CalculatePlotter( StepsPerPixelY * PictureHeight, 0, stepPauseNormal );//X-Y movement swapped!!!
            for(currentPlotY = 0; currentPlotY < PictureHeight; currentPlotY++){
              NewLine = 0;
              if(ReverseMode == 0){
                currentPlotX = 0;
              }
              else{
                currentPlotX = PictureWidth - 1;
              }
              while(NewLine == 0){
                CyanDrops = 0;
                MagentaDrops = 0;
                YellowDrops = 0;
                fseek(PlotFile, DataOffset + (currentPlotY * PictureWidth + currentPlotX) * 3, SEEK_SET);
                fread(&PixelBlue, 1, 1, PlotFile);
                fread(&PixelGreen, 1, 1, PlotFile);
                fread(&PixelRed, 1, 1, PlotFile);
                YellowDrops = (255 - PixelBlue) / 85;
                if(PixelBlue != 255 && PixelRed != 255 && PixelGreen != 255){
                  YellowDrops = 1;
                }
                else{
                  YellowDrops = 0;
                }
                SetDrops(CyanDrops, MagentaDrops, YellowDrops);
                if(ReverseMode == 0){
                  currentPlotX++;
                  if(currentPlotX < PictureWidth){
                    CalculatePlotter(0, StepsPerPixelX, stepPauseNormal);//X-Y movement swapped!!!                  
                  }
                  else{
                    NewLine = 1;
                    ReverseMode = 1;
                  }
                }
                else{
                  currentPlotX--;
                  if(currentPlotX > -1){
                    CalculatePlotter(0, -StepsPerPixelX, stepPauseNormal);//X-Y movement swapped!!!                  
                  }
                  else{
                    NewLine = 1;
                    ReverseMode = 0;
                  }                  
                }
              }//while(NewLine == 0){
              CalculatePlotter( -StepsPerPixelY, 0, stepPauseNormal );//X-Y movement swapped!!!
            }//for(currentPlotY = 0; currentPlotY < PictureHeight + JetOffset1 + JetOffset2; currentPlotY++){
            fclose(PlotFile);
          }//if(plotterMode == 2){
        }//if(strcmp(FileName, "noFiLE") != 0){
      }//if(KeyHit == 'p'){
      
    }//if(MenueLevel == 0){
    
    if(MenueLevel == 1){//Select file

      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 66 && KeyCode[3] == 0 && KeyCode[4] == 0){
        FileSelected++;
        strcpy(FileName, PrintMenue_02(FileStartRow, FileSelected, plotterMode));
      }

      if(KeyCode[0] == 27 && KeyCode[1] == 91 && KeyCode[2] == 65 && KeyCode[3] == 0 && KeyCode[4] == 0){
        if(FileSelected > 0){
          FileSelected--;
          strcpy(FileName, PrintMenue_02(FileStartRow, FileSelected, plotterMode));
        }
      }

      if(KeyHit == 10){//Read file and store values
        MenueLevel = 0;
        clrscr(MessageY + 1, MessageY + 1);
        strcpy(FullFileName, PICTUREPATH);
        strcat(FullFileName, "/");
        strcat(FullFileName, FileName);
        if((PlotFile=fopen(FullFileName,"rb"))==NULL){
          sprintf(TextLine, "Can't open file '%s'!\n", FullFileName);
          ErrorText(TextLine);
          strcpy(FileName, "NoFiLE");
        }
        else{
          xMin=1000000;
          xMax=-1000000;
          yMin=1000000;
          yMax=-1000000;
          coordinateCount = 0;
          if(plotterMode == 0 || plotterMode == 1){
            while(!(feof(PlotFile))){
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
                    xNow = strtol(TextLine, &pEnd, 10);
                    if(xNow > xMax){
                      xMax = xNow;
                    }
                    if(xNow < xMin){
                      xMin = xNow;
                    }
                    ReadState = 3;
                    i=0;
                    TextLine[0]='\0';
                  }
                }
              }
              if(ReadState == 3){
                if(strchr(IsDigit, a) == NULL){
                  if(strlen(TextLine) > 0){
                    yNow = strtol(TextLine, &pEnd, 10);
                    if(yNow > yMax){
                      yMax = yNow;
                    }
                    if(yNow < yMin){
                      yMin = yNow;
                    }
                    coordinateCount++;
                    ReadState = 2;
                    i=0;
                    TextLine[0]='\0';
                  }
                }
              }
              if(strcmp(TextLine, "<path") == 0){
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
              gotoxy(1, MessageY);printf("ReadState=% 3d, xNow=% 10ld, xMin=% 10ld, xMax=% 10ld, yMin=% 10ld, yMax=% 10ld   ", ReadState, xNow, xMin, xMax, yMin, yMax);
            }
            fclose(PlotFile);
          }//if(plotterMode == 0 || plotterMode == 1){            
          if(plotterMode == 2){//bitmap
            fread(&FileInfo, 2, 1, PlotFile);
            fread(&FileSize, 4, 1, PlotFile);
            fread(&LongTemp, 4, 1, PlotFile);
            fread(&DataOffset, 4, 1, PlotFile);
            fread(&HeaderSize, 4, 1, PlotFile);
            fread(&PictureWidth, 4, 1, PlotFile);
            fread(&PictureHeight, 4, 1, PlotFile);
            fread(&IntTemp, 2, 1, PlotFile);
            fread(&ColorDepth, 2, 1, PlotFile);
            fread(&CompressionType, 4, 1, PlotFile);
            fread(&PictureSize, 4, 1, PlotFile);
            fread(&XPixelPerMeter, 4, 1, PlotFile);
            fread(&YPixelPerMeter, 4, 1, PlotFile);
            fread(&ColorNumber, 4, 1, PlotFile);
            fread(&ColorUsed, 4, 1, PlotFile);
            if(FileInfo[0] != 'B' || FileInfo[1] != 'M'){
              sprintf(TextLine, "Wrong Fileinfo: %s (BM)!\n", FileInfo);
              ErrorText(TextLine);
              strcpy(FileName, "NoFiLE");
            }
            if(ColorDepth != 24){
              sprintf(TextLine, "Wrong ColorDepth: %d (24)!\n", ColorDepth);
              ErrorText(TextLine);
              strcpy(FileName, "NoFiLE");
            }
            if(CompressionType != 0){
              sprintf(TextLine, "Wrong CompressionType: %ld (0)!\n", CompressionType);
              ErrorText(TextLine);
              strcpy(FileName, "NoFiLE");
            }
            xMin=0;
            xMax=PictureWidth * StepsPerPixelX * 0.33333;
            yMin=0;
            yMax=PictureHeight * StepsPerPixelY * 0.33333;
            coordinateCount = PictureWidth * PictureHeight;
          }
        }
        PicWidth = (double)(xMax - xMin) * Scale / 1000.0;
        PicHeight = (double)(yMax - yMin) * Scale / 1000.0;
        PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
      }//if(KeyHit == 10){
    
    }//if(MenueLevel == 1){
        
    if(KeyHit == 27){
      if(MenueLevel == 0){
        clrscr(MessageY + 1, MessageY + 1);
        MessageText("Exit program (y/n)?", MessageX, MessageY + 1, 0);
        while(KeyHit != 'y' && KeyHit != 'n'){
          KeyHit = getch();
          if(KeyHit == 'y'){
            for(i=0;i<14;i++){
              digitalWrite(GPIO[i], 0);
            }
            exit(0);
          }
        }
      }
      if(MenueLevel == 1){
        MenueLevel = 0;
        strcpy(FileName, FileNameOld);
        PrintMenue_01(FileName, Scale, PicWidth, PicHeight, MoveLength, plotterMode);
      }
      clrscr(MessageY + 1, MessageY + 1);
    }
  }

  return 0;
}
