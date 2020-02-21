#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <ctime>

std::vector<std::string> splitString(std::string text, char d);
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list);
bool fileExists(std::string full_path, bool *executable);

int main (int argc, char **argv)
{
    	char* os_path = getenv("PATH");
    	std::vector<std::string> os_path_list = splitString(os_path, ':');
	std::cout << "Welcome to OSShell! Please enter your commands ('exit' to quit)." << std::endl;
	bool executable;
	std::string input;
	std::string fullpath;

	std::vector<std::string> history_list;
	std::vector<std::string> spliter;
	std::vector<char*> args;

	time_t now = time(0);
	char *dt;
  	pid_t c_pid, pid;
  	int status;
    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //   If yes, execute it
    //   If no, print error statement: "<command_name>: Error running command" (do include newline)
	while( 1 ){

		std::cout << "osshell>";
    		std::getline(std::cin, input);
		std::cin.clear();
		args.clear();
		if(input.compare("exit")==0){ break; }

		spliter = splitString(input, ' ');

		for(int i=0; i<spliter.size(); i++){
			if(i==0){
   			  	size_t found = spliter[i].find_last_of("/\\");
  			  	std::string path = spliter[i].substr(found+1); // check that is OK
				fullpath = getFullPath(path, os_path_list);
				args.push_back( const_cast<char*>(fullpath.c_str()) );
			}else{
				args.push_back( const_cast<char*>(spliter[i].c_str()) );
			}
		}
		args.push_back(0);
		history_list.push_back(input);
	
		if(  fileExists(fullpath, &executable) && executable ){
			c_pid = fork();
  			if (c_pid == 0){
    			/* CHILD */
			//printf("Child: executing ls\n");
    			//execute     
                                                                                                       
			execv(args[0], &args.front());
    			//only get here if exec failed
                       
    			perror("<command_name>: Error running command");
  			}else if (c_pid > 0){
    			/* PARENT */
    			if( (pid = wait(&status)) < 0){
				//getpid();
      			perror("wait");
      			_exit(1);
    			}
    			//printf("Parent: finished\n");
  			}else{
    			perror("fork failed");
    			_exit(1);
  			}

		}else{
			std::cout << input << ": Error running command" << std::endl;
		}
		std::cin.clear();
		fullpath.clear();
		input.clear();
	}
	
    return 0;
}

// Returns vector of strings created by splitting `text` on every occurance of `d`
std::vector<std::string> splitString(std::string text, char d)
{
    std::vector<std::string> result;
	std::string token;
	std::stringstream tokenStream(text);
	while(std::getline(tokenStream, token, d)){
		result.push_back(token);
	}
    return result;
}

// Returns a string for the full path of a command if it is found in PATH, otherwise simply return ""
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list)
{
    	DIR *dp;
    	struct dirent *dirp;
	std::string fname;
	for(int i=0; i<os_path_list.size(); i++){
	    	if((dp = opendir(os_path_list[i].c_str())) == NULL)
    		{
      		//std::cout << "Error(" << errno << ") opening " << cmd << std::endl;
      		//return errno;
    		}
    		while ((dirp = readdir(dp)) != NULL){
			fname = dirp->d_name;
			if( fname.compare(cmd)==0 ){
				return os_path_list[i] + "/" + fname ;
			}
			fname.clear();
    		}
	}
	closedir(dp);
    return "";
}

// Returns whether a file exists or not; should also set *executable to true/false 
// depending on if the user has permission to execute the file
bool fileExists(std::string full_path, bool *executable)
{
    	*executable = false;
    	std::ifstream f(full_path);
	if(f.good()){
		if( !access(&full_path[0], X_OK) ){ *executable = true; }
		return true;
	}else{
		return false;
	}
	return false;
}
