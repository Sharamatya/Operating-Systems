#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#define MAX_CHAR 100
using namespace std;


// print any error occured
void error( string msg ){
		const char *mesg;
		mesg = msg.c_str();	
		perror( mesg );
		exit(1);
}

// parses the input tokens
int parser( vector<string> &tokens,string &input_file, string &output_file, int &i_flag, int &o_flag, int &bg, int &len){
	   // iterate over tokens to check for i/p o/p redirection or background processing command
    // ++it lets us skip first word
	for(auto it = tokens.begin(); it!=tokens.end(); ++it )
        {
            if((*it)=="<")
            {
                i_flag =1;
                input_file = *(it+1);
            }
            if((*it)==">")
            {
                o_flag=1;
                output_file = *(it+1);
            }
            if((*it)=="&")
            {
                bg=1;
            }
            if(!i_flag&&!o_flag&&!bg)
                len++;
        }
    return 0;
}
// execute the command passed from parser
void execute(string sentence, int in_fd, int out_fd, int pre_out, int next_in) {
    istringstream iss(sentence);

    vector<string> tokens(istream_iterator<string>(iss), istream_iterator<string>{});	// place words into vector tokens from sentence
    int l =0, r=0, bg=0,end=0;
    string l_file, r_file;		// file names to be read in case of i/p o/p redirection
 

        parser(tokens, l_file, r_file, l, r, bg, end);	
        const char **argv = new const char *[end+1];

        for (int i = 0; i < end; i++)
        {
            argv[i] = tokens[i].c_str();
        }

        argv[end] = NULL;
        
        pid_t child_pid = fork();
        if ( child_pid == 0)
        {

            if(r){// output redirection
            	// sanity check if file's opened or not
                out_fd = open(r_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
                if(out_fd == -1){
                	error("OUTPUT FILE ERROR ");
                	// printf("f1\n");
                }
                // else{printf("f2");}
                
            }
           
            if(out_fd!=1) {// if the output isn't standard output redirect
                if(dup2(out_fd, 1) == -1)
                	error("OUTPUT REDIRECTION ERROR ");
            	// printf("f3");
            }

            if(l){// input rediredction
            	in_fd = open(l_file.c_str(), O_RDONLY);
                if(in_fd == -1){
                	error("INPUT FILE ERROR ");
                }
                else{};
            }
            
            if(in_fd!=0) {// if the input isn't standard input reduirect
                if(dup2(in_fd, 0) == -1)
                	error("INPUT REDIRECTION ERROR");
            }
            
            execvp(tokens[0].c_str(), (char **)argv);
            
            exit(0);
        }
        
        else
        {	
            // wait untill child process is not over
            if(bg)
            {
                cout << " Child process running in background \n";
            }
            else
            {
                wait(NULL);
                // printf("f6\n");
                if(in_fd!=0)
                    close(in_fd);
                if(out_fd!=1)
                    close(out_fd);

            }
        }
    return;
}

int main()
{
    while (true)
    {
        vector<string> input;
        int beg = 0, end  = 0, len , pipes_number,i, in_fd, out_fd, pre_out, next_in;
        string line;
        int **pipes;
        cout << " shell ";
        getline(cin, line); 

        //Break on getting "exit"
        if (line == "exit")
            break;
        for(i=0;;i++)
        {
            if(line[i]=='\0')
                break;
            else if(line[i]=='|') 
            {
                len = end - beg;
                input.push_back(line.substr(beg, len));
                end = i+1;
                beg = i+1;
            }
            else
                end++;
        }
        len = end - beg;
        input.push_back(line.substr(beg, len));
        pipes = new int *[input.size()-1];
        for(i=0;i<input.size()-1;i++)
        {
            pipes[i]=new int[2];
            pipe(pipes[i]);
        }
        for(i=0;i<input.size();i++) 
        {
            in_fd = 0;
            out_fd = 1;
            pre_out = -1;
            next_in = -1;
            if(i < input.size()-1 ) 
            {
                out_fd = pipes[i][1];
                next_in = pipes[i][0];
            }
            if(i>0) 
            {
                in_fd = pipes[i-1][0];
                pre_out = pipes[i-1][1];
            }
            execute(input[i], in_fd, out_fd, pre_out, next_in);

        }
       fflush(stdin);
    }
    return 0;
}