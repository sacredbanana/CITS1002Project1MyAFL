/* 				*
 * CITS1002 Project 1 2012	*
 * Name: Cameron Armstrong	*
 * Student number: 21194619	*
 * Date: 21st September 2012	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#define TEAMS 24
#define TEAMNAMESIZE 30

char teamNames[TEAMS - 1][TEAMNAMESIZE]; // Create global variables
char scores[250][100];
int currentGoalsOne;
int currentBehindsOne;
int currentGoalsTwo;
int currentBehindsTwo;
char currentTeamOne[TEAMNAMESIZE] = "", currentTeamTwo[TEAMNAMESIZE] = "";

static void trimLine(char line[]) // Trims line from the newline character
{
int i = 0;
while (line[i] != '\0')
{
	if( line[i] == '\r' || line[i] == '\n' )
        {
            line[i] = '\0'; // overwrites with null byte
            break;          
        }
        i++;   
}
}

static void readTeamNames(char tf[]) // Gets a list of teamnames from the file
{
FILE *nameFile;
nameFile = fopen(tf, "r");
if (nameFile == NULL) // Check for error reading file
{
	fprintf(stderr, "Cannot open teamname file '%s'\n", tf);
	exit(EXIT_FAILURE);
}
char line[BUFSIZ];
int i = 0;
while ( fgets(line, sizeof line, nameFile) != NULL ) // Read file line by line
{
	trimLine(line);
	strcpy(teamNames[i], line);
	i++;
}	
fclose(nameFile);
return;
}

static int readScores(char sf[]) // Reads the score file
{
FILE *scoreFile;
scoreFile = fopen(sf, "r");
if (scoreFile == NULL) // Checks for error reading file
{
	fprintf(stderr, "Cannot open score file '%s'\n", sf);
	exit(EXIT_FAILURE);
}
char line[BUFSIZ];
int i = 0;
while ( fgets(line, sizeof line, scoreFile) != NULL ) // Read file line by line
{
	strcpy(scores[i], line);
	i++;
}
fclose(scoreFile);
return i; // Returns number of lines in the file
}

static int teamCounter() // Counts the number of teams
{
int i = 0;
while (teamNames[i][0] != '\0')
{
	i++;
}
return i;
}

static bool processScoreLine(int i) // Does processing of the data extracted from the scores file
{
int j = 0;
bool newLine = false;
char currentGoalsStrOne[2] = "", currBStrOne[2] = "", currentGoalsStrTwo[2] = "", currentBStrTwo[2] = "";
strcpy(currentTeamOne, "");
strcpy(currentTeamTwo, "");
char character[1];
char string[4];
if (scores[i][0] == '\n') // Checks to see if the line consists of a sole new line character to avoid being processed
{
	newLine = true;
	return newLine;
}
else
{		
	while (scores[i][j] != ' ')	// Process first team
	{
		character[0] = scores[i][j]; // Process teamname
		strcat( currentTeamOne, character);
		j++;
	}
	while (scores[i][j] == ' ')
		j++;
	while (scores[i][j] != ' ') // Process goals
	{
                character[0] = scores[i][j];
		strcat( currentGoalsStrOne, character);
		j++;
      	}
	while (scores[i][j] == ' ')
		j++;
	while (scores[i][j] != ' ') // Process behinds
	{
                character[0] = scores[i][j];
		strcat( currBStrOne, character);
		j++;
       	}
	while (scores[i][j] == ' ')
		j++;
	while (scores[i][j] != ' ')	// Process second team
        {
                character[0] = scores[i][j]; // Process teamname
                strcat( currentTeamTwo, character);
                j++;
		strcpy(string,currentBStrTwo);
        }
        while (scores[i][j] == ' ')
                j++;
        while (scores[i][j] != ' ') // Process goals
        {
                character[0] = scores[i][j];
                strcat( currentGoalsStrTwo, character);
                j++;
        }
        while (scores[i][j] == ' ')
                j++;
        char cbs[2] = "";
	while (scores[i][j] != '\n') // Process behinds
        {
              	character[0] = scores[i][j];
		strcat( cbs, character);
                j++;
	}
currentGoalsOne = atoi(currentGoalsStrOne); // Convert strings to integers
currentBehindsOne = atoi(currBStrOne);
currentGoalsTwo = atoi(currentGoalsStrTwo);
currentBehindsTwo = atoi(cbs);
return newLine;
}
}

static int behindsCalc( int g, int b) // Calculates the total number of behinds
{
b = b + (6 * g); 
return b;
}

static float percentageCalc( int f, int a) // Calculates the percentage
{
float p;
p = (float)f / (float)a * 100.0;
return p;
}

static int premiershipCalc( int w, int d) // Calculates the premiership points
{
int p;
p = (4 * w) + (2 * d);
return p;
}

static void exportFile( char of[], char tn[]) // Exports the output file
{
FILE *outputFile;
outputFile = fopen(of, "a");
if(fputs(tn, outputFile) == 0) // Checks for an error in creating the file
            {
                printf("error copying file\n");
                exit(EXIT_FAILURE);
            }
fclose(outputFile);
}

static void sortFile( char of[]) // Sorts the file by premiership points
{
int pid = fork();
if (pid == 0)
{
char *newargs[] = {
		"/usr/bin/sort",
                of,
                "-n",
                "-r",
                "-k",
                "9",
		"-k",
		"6",
		"-o",
		of
                };
execv("/usr/bin/sort", newargs);
}
}

int main( int argc, char *argv[])
{
if (argc < 4) // Checks if enough arguments are supplied
{
	fprintf(stderr, "Must provide arguments: teamnames, scores, output.\n");
	exit(EXIT_FAILURE);
}
if (argc > 4) // Checks if too many arguments are supplied
{
	fprintf(stderr, "Too many arguments!\n");
	exit(EXIT_FAILURE);
}
char teamNameFile[50]; // Variables
char scoreFile[50];
char outputFile[50];
strcpy(teamNameFile, argv[1]);
strcpy(scoreFile, argv[2]);
strcpy(outputFile, argv[3]); 
readTeamNames( teamNameFile);
int noOfGames = 1;
int lineCount = readScores( scoreFile);
int noOfTeams = teamCounter();
int teamOne, teamTwo;
int totalBehinds[noOfTeams - 1];
int behinds[noOfTeams - 1];
int totalGoals[noOfTeams - 1];
int wins[noOfTeams - 1];
int losses[noOfTeams - 1];
int draws[noOfTeams - 1];
int behindsAgainst[noOfTeams - 1];
float percentage[noOfTeams - 1];
int premiershipPoints[noOfTeams - 1];
char data[6];
char outputLine[100];
for (int i = 0; i < noOfTeams; i++) // Initialise arrays
{
	behinds[i] = 0;
	totalBehinds[i] = 0;
	totalGoals[i] = 0;
	behindsAgainst[i] = 0;
	wins[i] = 0;
	losses[i] = 0;
	draws[i] = 0;
	percentage[i] = 0.0;
	premiershipPoints[i] = 0;
}
for (int lineNo = 0; lineNo <= (lineCount - 1); lineNo++)
{
	if (processScoreLine(lineNo) == false) // Checks to make sure the line wasn't just a new line character
	{
		for (int i = 0; i < noOfTeams; i++) // Process the read data lines
		{
			if (strcmp(teamNames[i], currentTeamOne) == 0)
				(teamOne = i);
			if (strcmp(teamNames[i], currentTeamTwo) == 0)
				(teamTwo = i);
		}
		totalGoals[teamOne] = totalGoals[teamOne] + currentGoalsOne;
		totalGoals[teamTwo] = totalGoals[teamTwo] + currentGoalsTwo;
		behinds[teamOne] = behinds[teamOne] + currentBehindsOne;
		behinds[teamTwo] = behinds[teamTwo] + currentBehindsTwo;
		totalBehinds[teamOne] = behindsCalc( currentGoalsOne, currentBehindsOne);
		totalBehinds[teamTwo] = behindsCalc( currentGoalsTwo, currentBehindsTwo);
		behindsAgainst[teamOne] = behindsAgainst[teamOne] + totalBehinds[teamTwo];
		behindsAgainst[teamTwo] = behindsAgainst[teamTwo] + totalBehinds[teamOne];
		if (totalBehinds[teamOne] > totalBehinds[teamTwo])
		{	
			wins[teamOne]++;
			losses[teamTwo]++;
		}
		if (totalBehinds[teamOne] < totalBehinds[teamTwo])
		{
			wins[teamTwo]++;
			losses[teamOne]++;
		}
		if (totalBehinds[teamOne] == totalBehinds[teamTwo])
		{
			draws[teamOne]++;
			draws[teamTwo]++;
		}	
	}
	else
		noOfGames++;
}

for (int i = 0; i < noOfTeams; i++) // Build lines to write to file
{
	totalBehinds[i] = behindsCalc( totalGoals[i], behinds[i]); // Do calculations
	percentage[i] = percentageCalc( totalBehinds[i], behindsAgainst[i]);
	premiershipPoints[i] = premiershipCalc( wins[i], draws[i]);
	strcpy(outputLine, teamNames[i]);
	strcat(outputLine, " ");
	sprintf(data, "%d", noOfGames);	
	strcat(outputLine, data);
	strcat(outputLine, " ");
	sprintf(data, "%d", wins[i]);
        strcat(outputLine, data);
        strcat(outputLine, " ");
        sprintf(data, "%d", losses[i]);
        strcat(outputLine, data);
        strcat(outputLine, " ");
        sprintf(data, "%d", draws[i]);
        strcat(outputLine, data);
        strcat(outputLine, " ");
        sprintf(data, "%d", totalBehinds[i]);
        strcat(outputLine, data);
        strcat(outputLine, " ");
        sprintf(data, "%d", behindsAgainst[i]);
        strcat(outputLine, data);
        strcat(outputLine, " ");
        sprintf(data, "%.2f", percentage[i]);
        strcat(outputLine, data);
        strcat(outputLine, " ");
        sprintf(data, "%d", premiershipPoints[i]);
        strcat(outputLine, data);
	strcat(outputLine, "\n");
	exportFile( outputFile, outputLine); // Export file
}
sortFile(outputFile);
return 0;
}
