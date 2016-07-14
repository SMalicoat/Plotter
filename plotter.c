#include <stdio.h>
#include <dirent.h>
#include <wiringPi.h>
#include <unistd.h>
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
void clearScreen()
{
	const char* CLEAR_SCREE_ANSI = "\e[1;1H\e[2J";
	write(STDOUT_FILENO,CLEAR_SCREE_ANSI,12);
}
main()
{

	//wiringPiSetupGpio(): //need to do this once i start working with the
		//pi need to look into what pin number sceme to use


	FILE * fp;
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
		printf("%s",line);
	}
	
	fclose(fp);
//	if(line)
//		free(line);
	
	printf("\n\n");	
	getchar();
	clearScreen();
	DIR *dp;
	struct dirent *ep;
	dp = opendir ("./");
	if(dp != NULL)
	{
		while (ep = readdir (dp))
			if(ep->d_name[0]!='.')
				puts(ep->d_name);
		(void) close(dp);
	}
	else 
		perror("Could't open the directory");
	

	return 0;
}

