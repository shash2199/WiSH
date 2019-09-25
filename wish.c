#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int piping(char*);
void erase_path(void);
void print_history(int);
void push(char*);
void error_code (void);
void wisc_shell (char*, char*);
typedef struct {
	char val[10000];
	struct Node* next;
} Node;
 Node* head = NULL;
 Node* tail = NULL;
int num_nodes = 0;
char** path;
int num_paths = 1;
// push to the head
void push(char* cmd) {
    Node* node = malloc(sizeof(Node));
//	node->val = cmd;
//    if (head != NULL)
//	printf("preb %s %p\n", head->val, head->next);
	if (head == NULL) {
		head = node;
	    strcpy(head->val, cmd);
        head->next = NULL;
        tail = head;// edited
	}
	else {
		// node->next = (struct Node*)head;
        // strcpy(node->val, cmd);
		// head = node;
        tail->next = (struct Node*)node;
        tail = (Node*)tail->next;
        strcpy(tail->val, cmd);
        tail->next = NULL;
	}
	num_nodes++;
//	printf("%s %p\n", head->val, head->next);
//	printf("%s %p\n", node->val, node->next);
}

// print all the history in order
// n must be a number at this point
void print_history(int n) {
	if (head == NULL)
		return;
	Node* current = head;
	if (n > num_nodes)
		while (current != NULL) {
            // printf("%s %p\n", current->val, current->next);
//           printf("I am here\n");
           write(STDOUT_FILENO, current->val, strlen(current->val));
			current = (Node*)current->next;
		}
	else {
        Node* curr = head;
        int i = num_nodes - n;
        while (i > 0) {
            curr = (Node*) curr->next;
            i--;
        }
		while (n > 0) {
			// printf("%s %p\n", current->val, current->next);
//		    printf("Hello, here!\n");
            write(STDOUT_FILENO, curr->val, strlen(curr->val));
            curr = (Node*)curr->next;
			n--;
		}

		
	}

}

// Return 1 if | is found
int piping (char* cmd) {
    int num_pipe = 0;
    int f = 0;
    
    for (int i = 0; i < strlen(cmd); i++) {
        if ((cmd[i] ==  '|')) {
            num_pipe++;
            if (f == 0) {
                error_code();
                return 1;
            }
                
            if (i == strlen(cmd - 1)) {
                error_code();
                return 1;
            }
            if (i == 0) {
                error_code();
                return 1;
            }
        } else if (cmd[i] == '\t' || cmd[i] == ' ' || cmd[i] == '\n') {
            continue;
        }
        else {
            f = 1;
            if (num_pipe == 1)
                f = 9;
        }
        
    }
    
    if (num_pipe > 1) {
        error_code();
        return 1;
    }
    else if (num_pipe == 0 ) {
        
        return 0;
    }
    else if (f != 9) {
        error_code();
        return 1;
    }

    // check if we have a pipe
    char temp_cmd[strlen(cmd)];
    strcpy(temp_cmd, cmd);
    char* delim = "|";
    char* next_val = strtok(temp_cmd, delim);
// pipe present
    char* second_part = strtok(NULL, delim);
    // check if NULL precedes pipe
    next_val = strtok(next_val," \n\t");
    if (strlen(next_val) == 0) {
        error_code();
        return 1;
   } 
    strcpy(temp_cmd, cmd);
    next_val = strtok(temp_cmd, delim);
    // check if two pipes present
    char* nextnext = strtok(NULL, delim);
    // 
    nextnext = strtok(second_part, " \t\n");
    if(strlen(nextnext) == 0) {
        error_code();
        return 1;
    }

    strcpy(temp_cmd, cmd);
    char* next = strtok(temp_cmd, ">");
    if (strcmp(next, cmd) != 0) {

        error_code();
        return 1;
    }
    char check[strlen(cmd)];
    strcpy(check, cmd);
        int a1 = 0;
        int a2 = 0;
       // int flag = 0;
        char* args1[strlen(cmd)];
        char* args2[strlen(cmd)];
        delim = " \n\t";
        next_val = strtok(cmd, "|");
        // puts(next_val);
        // puts(nextnext);
        char* token1 = strtok(next_val, delim);
        do {
            args1[a1] = token1;
            a1++;
      //      puts(args1[a1 - 1]);
        } while ((token1 = strtok(NULL, delim)) != NULL);
        token1 = strtok(nextnext, delim);
        do {
            args2[a2] = token1;
            a2++;
        //    puts(args2[a2-1]);
        } while ((token1 = strtok(NULL, delim)) != NULL);
        args1[a1] = NULL;
        args2[a2] = NULL;
        // check, cmd, temp_cmd contain the same values
        fflush(NULL);
       //  return 1;
      //  puts("enter"); 
        
        fflush(NULL);
        int flag1 = 0;
        int flag2 = 0;
        for (int i = 0; i < num_paths; i++) {
            char pname1[strlen(path[i]) + strlen(args1[0]) + 5];
            strcpy(pname1, path[i]);
            if (path[i][strlen(path[i]) - 1] != '/')
                strcat(pname1, "/");
            strcat(pname1, args1[0]);
            char pname2[strlen(path[i]) + strlen(args2[0]) + 5];
             strcpy(pname2, path[i]);
            if (path[i][strlen(path[i]) - 1] != '/')
                strcat(pname2, "/");
            strcat(pname2, args2[0]);
            if (flag1 == 0) {
                if (access(pname1, X_OK) == 0) {
                    flag1 = 1;
                    
                    args1[0] = malloc(sizeof(char*) * strlen(pname1));
                    strcpy(args1[0], pname1);

                }
            }
            if (flag2 == 0) {
                if (access(pname2, X_OK) == 0) {
                    flag2 = 1;
                      args2[0] = malloc(sizeof(char*) * strlen(pname2));
                    strcpy(args2[0], pname2);


                }
            }
        
        }
        int rc1 = fork();
        if (rc1 < 0) {
    //        puts("the forking failed");
            error_code();
            return 1;
        }
        if (rc1 == 0) {
        // child process
            int fd[2];
            if (pipe(fd) < 0) {
      //          puts("Child failed in pipe");
                error_code();
                exit(0);
            }
            int rc2 = fork();
            if (rc2 < 0) {
        //        puts("Grand child failed");
                error_code();
                exit(0);
            }
            if (rc2 == 0) {
                // grand child
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                // TODO
               if( execv(args1[0], args1) == -1) {
     //              puts("execv in grand child failed");
                   error_code();
                   exit(0);

                }
            }
            else {
                wait(NULL);
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                if(execv(args2[0], args2) == -1) {
    //                puts("Child failed in execv");
                    error_code();
                    exit(0);
                }
                exit(0);
            }

        }
        else {
        // parent process
        wait(NULL);


        }
       
    

    return 1;
            

}

void erase_history() {
    Node* curr = head;
    while (num_nodes > 0) {
        curr = (Node*)head->next;
        free(head);
        head = curr;
        num_nodes--;
    }
erase_path();
}

void erase_path() {
//    if(path != NULL)
    for (int i = 0; i < num_paths; i++)
//        printf("%s\n", path[i]);
        free(path[i]);

    if (path != NULL)
        free(path);
    path = NULL;
    num_paths = 0;
}

int main (int argc, char* argv[]) {
	if (argc > 2) {
	//	printf("Only one or two Arguments Allowed!\n");
        error_code();
		exit(1);
	}
	char* end = "exit";
	char* cmd = argv[0];
        size_t n = 0;
//	char* str = "/bin\0";

    path = malloc(sizeof(char*)*num_paths);
 //   path[0] = "/bin\0";
   path[0] = malloc(sizeof(char*) * strlen("/bin\0"));
    strcpy(path[0], "/bin\0");
       // interactive mode
        if (argc == 1) {
                while (1) {
                    if ((char*)cmd != (char*)argv[0])
                            free(cmd);
                    printf("wish> ");
                    fflush(NULL);
                    cmd = NULL;
                    n = 0;
                    ssize_t char_read = getline(&cmd, &n, stdin);
                   if (strcmp(cmd, "\n") == 0) 
                       continue;
                   push(cmd);
                    char full_cmd[strlen(cmd)];
                    strcpy(full_cmd, cmd);
	                fflush(NULL);	
			if (char_read == -1) {
				free(cmd);
				exit(0);
			}
			char* delim = " \t\n";
			char* command = strtok(cmd, delim);

			if (command == NULL)
				continue;

			if (strcmp(end, command) == 0) {
				free(cmd);
                erase_history();
				exit(0);
			}
			// it is now assumed that the command is not exit
			wisc_shell(command, full_cmd);
                }
        }
        // batch mode
        if (argc == 2) {

                FILE* file = fopen(argv[1], "r");
                if (file == NULL) {
           //             printf("File NOT Found\n");
                        error_code();
                        exit(1);
                }
                while (1) {
                        if ((char*)cmd != (char*)argv[0])
                                free(cmd);
                        cmd = NULL;
                        n = 0;
                        ssize_t char_read = getline(&cmd, &n, file);
                         if (strcmp(cmd, "\n") == 0) 
                             continue;
                       push(cmd);
                    char full_cmd[strlen(cmd)];
                    strcpy(full_cmd, cmd);
			if (char_read == -1) {
				free(cmd);
				exit(0);
			}
			char* delim = " \t\n";
			char* command = strtok(cmd, delim);
			if (command == NULL)
				continue;

			if (strcmp(end, command) == 0) {
				free(cmd);
                erase_history();
                if (fclose(file) != 0) {
                     // printf("Error closing the file\n");
                       error_code();                 
                       erase_history();
                       exit(1);
                       }
                 exit(0);
			}
			// cmd is not exit 
			wisc_shell(command, full_cmd);
                }
        }
        return 0;
}

void wisc_shell (char* cmd, char* full_cmd) {
	char* delim = " \t\n";
	char* cd = "cd";
	if (strcmp(cmd, "cd\n") == 0) {
		// puts("here:");
		error_code();
		return;
	}	
	// implementation of cd
	if (strcmp(cmd, cd) == 0) {
		char* command;
		command = strtok(NULL, delim);
		if (command == NULL) {	
		    	error_code();
			return;
		}
		char* next = strtok(NULL, delim);
		if (next != NULL) {
	 	    error_code();
		    return;
		}
		// char dest[strlen(cmd) + 1];
		// strcpy(dest, cmd);
	       // strcpy(add, "/");

		int dir = chdir(command);
		if (dir == -1) {
			error_code();
			return;
		}
		return;

	}
	// history command
	if (strcmp(cmd, "history") == 0) {
		char* next_num = strtok(NULL, delim);
        if (next_num == NULL) {
            print_history(num_nodes);
            return;
        }
        char* next = strtok(NULL, delim);
        if (next != NULL) {
            error_code();
            return;
        }
        
        int dot = 0;
        for (int i = 0; i < strlen(next_num); i++) {
            int check = *(next_num + i);
            if ((check >= 48 && check <= 57) || check == '.') {
               if (check == '.')
                   if (i != (strlen(next_num) - 1)) {
                        if(next_num[i + 1] >= 48 && next_num[i + 1] <= 57)
                            dot = 1;
                   }

               continue;
           } else {
                error_code();
                return;
            }
        }
        print_history(atoi(next_num) + dot);
        return;
	}
     
    // path command
    if (strcmp(cmd, "path") == 0) {
        erase_path();
        int c = 0;
        char* next;
        while ((next = strtok(NULL, delim)) != NULL) {
            c++;
        }
        // puts(full_cmd);
        path = malloc(sizeof(char*) * c);
        next = strtok(full_cmd, delim);
        int i = 0;
        while ((next = strtok(NULL, delim)) != NULL) {
           path[i] = malloc(sizeof(char) * (strlen(next) + 1));
           if (next[0] != '/')
               strcpy(path[i], "/");
           strcat(path[i], next);          
          // path[i] = next;
            i++;
        }
        num_paths = i;
      //  printf("%i\n", num_paths);
        // for (int j = 0; j < i; j++)
        //    puts(path[j]);
        return;
    }
    // system call time
    if (num_paths == 0) {
        error_code();
        return;
    }

    if(piping(full_cmd) == 1)
        return;


    // case: redirection in one word
    char temp_full_cmd[strlen(full_cmd) + 1];
    strcpy(temp_full_cmd, full_cmd);
    strtok(temp_full_cmd, delim);
    char* second_token = strtok(NULL, delim);
    char prev_cmd[strlen(full_cmd)];
    int flag_re = 0; 
    int lc1 = 0;
//    int lc2 = 0;
    char target_file[strlen(full_cmd) * 2];
    int num_re = 0;
    for (int v = 0; v < strlen(full_cmd); v++) {
        if (second_token != NULL) {
           // printf("second token  NULL\n");
           break;
        }
        if (full_cmd[v] == ' ' || full_cmd[v] == '\t' || full_cmd[v] == '\n') {
            continue;

        }
        else if (full_cmd[v] == '>') {
               if (flag_re == 0) {
                    error_code();
                    return;
               
               }
                num_re++;
               strcpy(prev_cmd, target_file);
               prev_cmd[lc1] = '\0';
               target_file[lc1] = ' ';
               target_file[lc1 + 1] = '>';
               target_file[lc1 + 2] = ' ';
               lc1 = lc1 + 3;
               }
        else {
               flag_re = 1;
               if (num_re > 0) {
                   target_file[lc1] = full_cmd[v];
                   lc1++;
                   flag_re = 9;
               }
               else {
                    target_file[lc1] = full_cmd[v];
                    lc1++;
               }
        }
        
   }
   

   if(num_re > 1 || (flag_re == 1 && num_re != 0)) {
        // puts("heello");
        error_code();
        return;
   }
   if (num_re == 1) {
        if (flag_re != 9 ) {
           //  puts("hi");
            error_code();
            return;
        }
        target_file[lc1] = '\0';
        full_cmd = target_file;
        cmd = prev_cmd;
   }
   // puts(cmd); puts(full_cmd);


//                    char str_ptr[1];
//                    str_ptr [0] = first_token[v];
//                if (strcmp (str_ptr, ">") == 0) {
//                    flag_redir++;
//                   // if () { // TODO
//
//                    //}
//                    if (v == strlen(full_cmd) - 1 || v == 0) {
//                        error_code();
//                        exit(0);
//
//                    }
//                
//                } else if (flag_redir == 0) {
//                    prev_cmd[lc1] = *(first_token + v);
//                    lc1++;
//                }
//                else {
//                    prev_cmd[lc1] = '\0';
//                    target_file[lc2] = *(first_token + v);
//                    lc2++;
//                }
//    }
//    if (flag_redir > 1) {
//        error_code();
//        return;
//    }
//    int temp_flagg = 1;
//    // now, we are sure the one word contains a >
//            if (flag_redir != 0) {
//                printf("you can in brother!\n");
//                for (int v = 0; v < num_paths; v++) {
//                    
//                    char fpname[strlen(path[v]) + lc1 + 1];
//                    strcat(fpname, path[v]);
//                    if (path[v][strlen(path[v]) - 1] != '/')
//                        strcat(fpname, "/");
//                    strcat(fpname, prev_cmd);
//                target_file[lc2] = '\0';     
//                    printf("FPNAME : %s\n", fpname);
//                if (access(fpname, X_OK) == 0) {
//                    puts("Accessed");
//                    printf("prrev %s\n", prev_cmd);
//                    printf("targert %s\n", target_file);
//                    temp_flagg = 1;
//                int rc = fork();
//                if (rc < 0) {
//                    puts("here");
//                    error_code();
//                    exit(0);
//                }
//                if (rc == 0) {
//                int file_desc = open(target_file, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
//                dup2(file_desc, STDOUT_FILENO); 
//                dup2(file_desc, STDERR_FILENO);
//                char* args[3];
//                args[0] = prev_cmd;
//                args[1] = NULL;
//                
//                if (close(file_desc) == -1) {
//                      error_code();
//                      exit(0);
//                }
//                execv(prev_cmd, args);
//                }
//                else { 
//                    wait(NULL);
//                    return;
//                }
//                }
//                else
//                {
//                    temp_flagg = 0;
//                }
//            }
//            }
//            if (temp_flagg == 0) {
//                error_code();
//                return;
//            }
//
//


    // case : normal system call
    int flag = 0;
    for (int i = 0; i < num_paths; i++) {
        char temo[strlen(path[i]) + strlen(cmd) + 1];
        strcpy(temo, path[i]);
        if (path[i][strlen(path[i]) - 1 ] !=  '/')
            strcat(temo, "/");
        strcat(temo, cmd);
//        puts(temo);
        if (access(temo, X_OK) == 0) {
                flag = 0; 
                // TODO: concat the name of the path with the command
                int rc = fork();
                if (rc < 0) {
                    // fork failed
                    error_code();
                    return;
                }
                else if (rc == 0) {
                    // child process
                    int len = strlen(path[i]) + strlen(cmd);
                    int count = 1;
                    char* next;
                    int redir = 0;
                    char* output_file;
                    char f_name_full[strlen(full_cmd) + 1];
                    strcpy(f_name_full, full_cmd);
                    char * ptr = f_name_full;
                    while ((next = strtok(ptr, delim)) != NULL) {
                        count++;
                        if (strcmp(next, ">") == 0 || redir != 0)
                            redir++;
                        if (redir == 2)
                            output_file = next;
                        ptr = NULL;
                    }
                    
                    // if redir is enabled, its value should be 2
                    if (redir != 0) {
                        if (redir != 2) {
                            error_code();
                            exit(0);
                        }
                        else if (strcmp(output_file, ">") == 0) {
                            error_code();
                            exit(0);
                        }
                    }
                    next = strtok(full_cmd, delim);
                    // next --> name of command 
                    char path_name[len+1];
                 //   if (path[i][strlen(path[i]) - 1] != '/') {
                        
                   //     strcpy(path_name, path[i]);
                  //      strcat(path_name, "/");
                  //      strcat(path_name, next);
                  //  }
                  //  else {
                  //      strcpy(path_name, path[i]);
                  //      strcat(path_name, next);
                   // }
                   //  puts(path_name);
                    char* arguments[count + 1];
                    int k = 0;
                   
                   
                 //  char prev_cmd[strlen(next)];
                    int flag_redir = 0; 
                    int lc1 = 0;
                    int lc2 = 0;
                    char target_file[strlen(next)];
                    if (count == 1) {
                        for (int v = 0; v < strlen(next); v++) {
                            if (strcmp ((next + v), ">") == 0) {
                                flag_redir++;


                            } else if (flag_redir == 0) {
                   //             prev_cmd[lc1] = *(next + v);
                                lc1++;
                            }
                            else {
                     //           prev_cmd[lc1] = '\0';
                                  target_file[lc2] = *(next + v);
                                lc2++;
                            }
                         }

                    }
                                    
                        
                        if (flag_redir != 0) {
                            target_file[lc2] = '\0';
                            next = ">";
                            redir = 2;
                            output_file = target_file;
                        }
                        else { }
                        
                    strcpy(path_name, temo);
                    do {
                        if (redir == 2) {
                            if (strcmp(next, ">") == 0) {
                                int file_desc = open(output_file, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                                dup2(file_desc, STDOUT_FILENO); 
                                dup2(file_desc, STDERR_FILENO);
                                if (close(file_desc) == -1) {
                                    error_code();
                                    exit(0);
                                }
                                break;
                            }
                
                        }
                        arguments[k] = next;
                      //  puts(arguments[k]);
                        k++;

                    } while ((next = strtok(NULL, delim)) != NULL);
                    
                    arguments[k] = NULL;
                  //  for (int m = 0; m <= k; m++)
                  //      printf("Here:\n%s\n", arguments[m]);
                    int cv = execv(path_name, arguments);
                    if (cv == -1) {
//                        puts("hello\n");
                        error_code(); 
                        exit(0);
                    }

                    

                }
                else {
                    // parent process
                    wait(NULL);

                }

                break;
        }
        else
            flag = 1;
    }
    if (flag == 1)
        error_code();


}

void error_code() {
	char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
}
