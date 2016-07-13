#include <stdio.h>
#include <dirent.h>
#include <wiringPi.h>

main()
{

	//wiringPiSetupGpio(): //need to do this once i start working with the
		//pi need to look into what pin number sceme to use


	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen ("testfile.txt","r");
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

