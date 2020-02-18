#include <iostream>
#include <cstdlib>
#include <string>
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
    std::string input;
    char* os_path = getenv("PATH");
    std::vector<std::string> os_path_list = splitString(os_path, ':');

	std::vector<std::string> history_list;
    std::cout << "Welcome to OSShell! Please enter your commands ('exit' to quit)." << std::endl;


	std::string fullpath = getFullPath("ls", os_path_list);
	bool *executable;
	fileExists(fullpath, executable);
	char * ls_args[] = { "/bin/ls" , "-a", NULL}; 
        //execv(   ls_args[0],     ls_args);
  pid_t c_pid, pid;
  int status;

  c_pid = fork();

  if (c_pid == 0){
    /* CHILD */

    printf("Child: executing ls\n");

    //execute ls                                                                                                                                                               
    execv( ls_args[0], ls_args);
    //only get here if exec failed                                                                                                                                             
    perror("execve failed");
  }else if (c_pid > 0){
    /* PARENT */

    if( (pid = wait(&status)) < 0){
      perror("wait");
      _exit(1);
    }

    printf("Parent: finished\n");

  }else{
    perror("fork failed");
    _exit(1);
  }



     /*pid_t pid;
     char *const arr[] = {"ls", "-l", "-R","-a", NULL};
	execv("ls", arr);*/

	/*char *bin_path = "ls";
	char *args[]={bin_path,"-a", "-s",NULL};
        execv(bin_path,args);*/

    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //   If yes, execute it
    //   If no, print error statement: "<command_name>: Error running command" (do include newline)
	time_t now = time(0);
	char *dt;
	while(1){
    		std::cout << "osshell>";
    		std::getline(std::cin, input);

		history_list.push_back(input);

		if(input.compare("exit")==0){break;}


		if(input.compare("ls")==0){
		}
		else if(input.compare("history")==0){
			for(int i=0; i<history_list.size(); i++){
				std::cout << "  " << i <<": "<< history_list[i] << std::endl;
			}
		}
		else if(input.compare("date")==0){
			dt = ctime(&now);
			std::cout << dt;
		}else{
			std::cout << input << ": Error running command" << std::endl;
		}
	}

	std::cout << input << std::endl;
	std::cout << os_path << std::endl;
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
	for(int i=0; i<result.size(); i++){
	std::cout << result[i] << std::endl;
	}

    return result;
}

// Returns a string for the full path of a command if it is found in PATH, otherwise simply return ""
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list)
{

	//std::string last_element(cmd.substr(cmd.rfind("/") + 1));
	//std::cout << last_element << "   HERE " << std::endl;
    DIR *dp;
    struct dirent *dirp;


      //files.push_back(string(dirp->d_name));
    //closedir(dp);

	for(int i=0; i<os_path_list.size(); i++){
		
	    	if((dp = opendir(os_path_list[i].c_str())) == NULL)
    		{
      		std::cout << "Error(" << errno << ") opening " << cmd << std::endl;
      		//return errno;
    		}
    		while ((dirp = readdir(dp)) != NULL){
			std::string fname = dirp->d_name;
			if( fname.find(cmd) !=  std::string::npos){
				std::cout << os_path_list[i] << "/"<< fname << "/"<< cmd << std::endl;
				return os_path_list[i] + "/" + fname + "/" + cmd;
			}
    		}
	}
    return "";
}

// Returns whether a file exists or not; should also set *executable to true/false 
// depending on if the user has permission to execute the file
bool fileExists(std::string full_path, bool *executable)
{
    *executable = false;
    std::ifstream f(full_path);
	std::cout << full_path << std::endl;
	if(f.good()){
		*executable = true;
		return true;
	}else{
		return false;
	}
}
