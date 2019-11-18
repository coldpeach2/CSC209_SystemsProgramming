#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"
/*run_make takes a target as its first argument. 
It finds the rule in the rules data structure corresponding to the target, and evaluates it. 
If target is NULL, then run_make will evaluate the first target in the rules list. A rule is evaluated in the following steps:

1. Update each of the dependencies. In other words, recursively evaluate each dependency rule.
	//Check to see if you need to execute the action based on the times of the dependencies and the target
2. Compare the last modified time for each of the dependencies to the target.
3. If the target does not exist, or at least one of the dependencies has a last modified time more recent than the target, execute the actions.
Last modified time

Use stat to get the last modified time (mtime) of a file. Read the man page to learn how to call stat. 
The field of the stat struct that you want is called st_mtim on Linux. It has different names on Mac OSX, 
so if you have a Mac, you will need to either work on the Linux machines or remember to change your code 
when you do the final tests on the teach.cs machines. It may also be different on Windows Ubuntu, so you 
should check. The struct timespec st_mtim has two fields, one for seconds (tv_sec) and one for nanoseconds 
(tv_nsec). You will need both of these values to find out which time is more recent.

Executing actions
To execute an action, you will need to use fork and exec to the run the command specified by an action. 
Fortunately in part 1, you already prepared the array of arguments to pass into exec. The parent process 
should wait for each child to complete. If the child terminates with an error code (1), the parent should 
also terminate. This means that the pmake will stop when it encounters an error. The actions for one 
rule are not executed in parallel; they are executed one after another.*/

int execute_action(char** command, int n) {
/*Part 2 executing actions
if I have two rules to evaluate this means I create a child for each rule to execute the action for that specific rule.
 And I should wait for these two child processes to execute before the parent process does anything else.

*/
	pid_t child_pid, pid;
	int status;
	child_pid = fork();
	if(child_pid < 0){
		perror("fork fail");
	}
	else if(child_pid == 0){
		command[n] = NULL;
		execvp(command[0], command);
	}else{
		pid = wait(&status);

	}
	return 0;

}

void execute_actions(Action *act) {
/*Part 2 executing actions
if I have two rules to evaluate this means I create a child for each rule to execute the action for that specific rule.
 And I should wait for these two child processes to execute before the parent process does anything else.

*/
	Action *curr = act;
	int i= 0;
	while(curr != NULL){
		if(curr->args == NULL){
		}
	i = 0;
	while(curr->args[i] != NULL){
		i++;
		}

		execute_action(curr->args, i);
		curr = curr->next_act;
	}

}


int get_time(char *filename) {
	// FILE NAME IS TARGET ATTRIBUTE IN RULE NODES
	struct stat fileStat;
    if(stat(filename, &fileStat) < 0) {
        return 1;

    }   
/*Use stat to get the last modified time (mtime) of a file.
helper function that evaluates the most recent time a file dependency of curr->target has been modified 
and compare the last modified times of the dependencies*/
return fileStat.st_mtime;
}

void evaluate_rule(Rule *head, Rule *rule, Dependency *dep, int pflag){
	Rule *curr = head;
	int status;

	// printf("%s\n", "trace");
	if(dep != NULL){
		if(pflag == 1){
			int r = fork();
			 if (r < 0) {
		    perror("fork");
		    exit(1);   
		  }else if (r==0){
		  	evaluate_rule(head, rule->next_rule, rule->next_rule->dependencies, pflag);
		  }else{
		  	waitpid(r, &status, 0);
		  }

		}
		else{
		// handles recursion
			evaluate_rule(head, rule->next_rule, rule->next_rule->dependencies, pflag);
	
		// evaluate_rule(head, dep->rule, dep->rule->dependencies);

		// compare get_time of target file (rule->target) and dep->rule_node->target i.e get_time(rule->target) != get_time(dep->rule_node->target)
		// if different or dependency file doesn't exist, then execute actions
		// else do nothing
			int rule_time = get_time(rule->target);
			int dep_time = get_time(dep->rule->target);
			if ( rule_time < dep_time || dep_time == 1){
				while(curr != NULL){
					if(strcmp(curr->target, dep->rule->target) == 0){
						if(curr->actions !=NULL){
							execute_actions(curr->actions);
						}
					}
					curr = curr->next_rule;
				}
			}
		}
	}
 /*recursively evaluate each dependency rule.
 returns a linked list of actions that need to be executed
 use helper function get_time to evaluate the most recent time a file has been modified
 Check to see if you need to execute the action based on the times of the dependencies and the target*/

}


void run_make(char *target, Rule *rules, int pflag) {
//run_make takes a target as its first argument. 
//It finds the rule in the rules data structure corresponding to the target, and evaluates it.

	/*find rule in rules data structure*/
	Rule *curr = rules;
	// takes care of null case
	if(target == NULL){
		evaluate_rule(rules, curr, curr->dependencies, pflag);
	}else{
		while(curr != NULL) {
			if (strcmp(curr->target, target) == 0) {
				evaluate_rule(rules, curr, curr->dependencies, pflag);
				break;
			}
			curr = curr->next_rule;
		}
	}	

}