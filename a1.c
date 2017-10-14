#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE 80 /*The Maximun length command*/

/*
*Function to free memory from char array, takes char *[] and its size to free it
*/
void freeMemory(char *ch[], int size){
	int i;
	for (i=0; i<size; i++){
		free(ch[i]);
	}
}

/*
*Method that converts a string into a positive int. retunrs -1 if not an int.
*/
int fromCharToInt(char *ch){	
	int dec=-1;
	int len;
	len=strlen(ch);
	int i;
	if (len>0){dec=0;}
	for (i=0; i<len; i++){
		dec = (dec * 10) + (ch[i]-'0');
		if (isdigit(dec)!=0){
			return -1;
		}
	}
	return dec;
}

/*
* History method. Only enters if History command is inputted. Takes the command, the history and their limits. Depending on the second argumen
*of the command array it will give different outputs.It retunrs 1 if command is correct and 0 if not.
*/
int  history_search(char *commands[], char *history[], int index_command, int index_history){
	/*
	*Variables
	*/
	const char *hstry= "history";
	const char *first="!!";
	const char *specific = "!";
	const int limit_index_search = 10;
	int index_start_search=0;
	int flag=0;
	int flag2=0;
	int flag3=0;
	int num;
	
	/*
	*checks if user is requesting case 1: last ten ; case 2: last one or case 3: specific one
	*/
	if (index_command==2&&(strcmp(commands[0], hstry)==0)&&(strcmp(commands[1], first)==0)){
		flag2=1;
	}
	else if (index_command==1&& (strcmp(commands[0], hstry)==0)){
		flag = 1;
	}
	else if(index_command==3&&(strcmp(commands[0], hstry)==0)&&(strcmp(commands[1], specific)==0)){
		num = fromCharToInt(commands[2]);
		if (num > 0 && num < 10){
			flag3=1;
		}
		else {printf("Command Not Found in History!, %d\n", num);}
	}

	/*
	* Calculates index since only last ten are available
	*/
	if (index_history>9) {index_start_search= index_history-limit_index_search;}
	
	/*
	*Once case has been determined it prints the outcome
	*/
	if (flag2){
		printf("%s\n", history[index_history-2]);
		return 1;
	}
	else if (flag){
		int i;
		for ( i=index_history-1; i>=index_start_search; i--){
                        printf("%d %s\n",(i+1),history[i]);
                }
		return 1;
	}
	else if (flag3){
		printf("%s\n", history[index_start_search - 1 + num] );
		return 1;
	}
	return 0;
}

int main(){

	static int should_run =1;
	char *args[80/2+1]; 	/*Command Line Arguments*/
	char *history[MAX_LINE];	/*stores history*/
	int history_count=0;	/*history count, points to the next cell*/
	const char delim =' '; 	/*for further tokenazation*/
	const char *run_together = "&"; 	/*constant for running parent anf child together*/
	const char *exit_signal = "exit";	/*constant for ending the program*/
	const char *hstry= "history"; 
	
	while(should_run){
		
		printf("CSCI3120 ->");
		int count=0; 	/*counter for args, points to the end of command NULL based on execvp()*/
		char *p;	/*stores the user input*/
		p = (char*) malloc(80); 	/*memory allocation for dynamic stdin*/
		fgets(p, 81, stdin); 	/*reads input from user*/
		int numChar = strlen(p);
		p[numChar-1]='\0';	 /*remove the newline character*/

		/*
		*Memory allocation for the arrays history and args
		*/
		args[count] = (char*) calloc(numChar+1, 1); 
		history[history_count] = (char*) calloc(numChar+1, 1);

		strcpy(history[history_count++], p); /*stores history information*/

		/*
		* Tokenazation of the string into readables commands
		*/
		args[count] = strtok(p, &delim);
		while(args[count]!=NULL){
			args[++count]= strtok(NULL, &delim);	
		}

		/*
		*checks for exit signal
		*/
		if ((strcmp(args[0], exit_signal))==0&&count==1){
			printf("exiting...\n");
                        should_run=0;
		}
	
		int waitForChild = 1;
		/*
		* checks if parent and child run at together
		*/
		if ((strcmp(args[count-1], run_together)==0)) { 
			waitForChild =0;
			args[--count]=NULL;
		}

		/*
		*creates child process if history fucntion is not invoked or exit signal not present
		*/
		if (should_run){
			int pid = fork();
			if (pid == 0){

				/*
                		*checks for the hisotory command before it reaches the execevp fubction in child process.
                		*/
				if (history_search(args, history, count, history_count) == 0){

					/*
					* prints error message if command is not Found
					*/
					if (execvp(args[0], args) == -1){ perror("Command Not Found");}
				}
				exit(0);		
			}
			else if (waitForChild){
					wait(NULL);
			}
		}	
		/*
		*Freeing memory from args[] and p
		*/
		//free(p);
	      //freeMemory(args, count);
	}
	//freeMemory(history, history_count);
	return 0;
}


