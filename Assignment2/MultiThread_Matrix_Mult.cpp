#include <iostream>
#include <dirent.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include "worker.h"

#define N 3
#define directory "test/"
using namespace std;
void print_vec(vector<account> accounts)
{
    for(int i =0; i<accounts.size(); i++)
        cout << "id =" << accounts[i].id << "   deposit =" << accounts[i].deposit << endl;

}
void print_string_vec(vector<string> in)
{
    ofstream out;
    out.open("output.txt", ofstream::app);
    for(int i =0; i< N; i++)
        out << in[i] << " ";
    out << endl;

    out.close();
}
int main()
{    
    int num_of_workers;
    cout << "please enter num of workers" << endl;
    cin >> num_of_workers;
    bool quit = false;
    pid_t collector_pid = fork();
    if(collector_pid == 0)
    {
        // cout << "collector" << endl;

        int count = 0;
        int out_printed =1 ;
        fd_set master_read;
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&master_read);

        FD_SET(STDIN_FILENO, &master_read);



        char s[300];
        int num, fd;
        
        vector<string> res;
        mknod(FIFO_NAME, S_IFIFO | 0666, 0);

        printf("waiting for writers...\n");
        fd = open(FIFO_NAME, O_RDONLY);
        cout << "fd=" <<fd << endl << endl;
        FD_SET(fd, &master_read);
        printf("got a writer\n");

        while (1)
        {
            read_fds = master_read;
            if (select(fd+1, &read_fds, NULL, NULL, NULL) == -1) {
                perror("select");
                exit(4);
            }
            if(FD_ISSET(STDIN_FILENO,&read_fds))
            {
                //get input;
                //cout << "oomadam STDIN_FILENO" << endl;
                // string command;
                // cin >> command;
                // if (command == "quit")
                // {
                //     cout << "tahe khate piade shin\n" ;
                //     quit = true;
                //     exit(0);
                // }
                exit(0);
                
            }
            if(FD_ISSET(fd, &read_fds))
            {
                count++;
                if ((num = read(fd, s, 300)) == -1)
                {
                    perror("read");
                 
                }
                else {
                    
                    s[num] = '\0';
                    // cout << "read ------" << s << "-----" << endl;
                    if(strlen(s) > 5)
                    {

                        res.push_back(s);
                    }
                }
              
            }
            if(res.size()>N)
            {
                // cout <<"count =" << count << endl;
                // cout << "----------" << out_printed << "-----------" << endl;
                count -=N;
                print_string_vec(res);
                out_printed++;
                res.erase(res.begin(), res.begin()+N);
            }
            
        
       }

         // cout << "endofcollector" << endl;    
    }    
    else
    {
        vector<pid_t> pids;
        vector<account> accounts;
        vector<string> file_names;
        DIR *dir;
        struct dirent *ent;
        int count = 0;
        if ((dir = opendir ("test")) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                   printf ("%s\n", ent->d_name);
                   if(count > 2)
                        file_names.push_back(ent->d_name);
                    count++;
            }
            closedir (dir);
        } else {
                /* could not open directory */
                perror ("");
                return EXIT_FAILURE;
        }  
        /*
        for(int i=3; i<file_names.size(); i++)
            eval( directory + file_names[i], accounts);
        cout << "=========================================================\n";
        print_vec(accounts);
        cout << "=========================================================\n";
        */
        
        int last_file = 0;
        
        int portion = file_names.size()/num_of_workers;
        // cout << "portion =" << portion << endl;
        int remainder = file_names.size()%num_of_workers;
        // cout << "remainder =" << remainder << endl;
        for(int i=0; i<num_of_workers; i++)
        {
            pid_t pid;
            int pfds[2];
            pipe(pfds);
            pid = fork();
           
            
            if(pid == 0)
            {
                //child
                // cout << "*******child********" << endl;
                // cout << "child " << getpid()<< endl;
                char file_name_buf[32];
                int read_bytes;
                int new_portion = portion;
                if(i<remainder)
                {
                    new_portion++;
                }
                for(int l=0; l<new_portion; l++)
                {
                    read(pfds[0], file_name_buf, 32);
                    // cout << "pid = " << getpid() << endl 
                    // << "reading from pipe " << pfds[0] << ", " << pfds[1] << endl
                    // << "read -" << file_name_buf << "-" << endl;
                    eval(file_name_buf, accounts);
                }
                close(pfds[0]);
                
    
                // cout << "********end child*******" << endl;
                exit(0);

            }
            else
            {
                //parent
                // cout << "*********parent*******" << endl;
                
                fd_set master_read, master_write, read_fds, write_fds;
                FD_ZERO(&master_read);
                FD_ZERO(&master_write);
                FD_SET(STDIN_FILENO, &master_read);
                FD_SET(pfds[1], &master_write);
                
                pids.push_back(pid);

                // cout << "Assigning files to process " << i << endl;
                // cout << "with pipe " << pfds[0] << ", " << pfds[1] << endl;
                
                bool flag = true;
                while(1)
                {
                    read_fds = master_read;
                    write_fds = master_write;
                    if (select(pfds[1]+1, &read_fds, &write_fds, NULL, NULL) == -1) {
                    perror("select");
                    exit(4);
                    }

                    if(FD_ISSET(STDIN_FILENO, &read_fds))
                    {
                        // cout << "oomadam STDIN_FILENO" << endl;
                        // cout << "quit = " << quit << endl;
                        if(quit)
                        {
                            wait(NULL);
                            break;
                        }
                        string command;
                        cin >> command;
                        if (command == "quit")
                        {
                            // cout << "tahe khate piade shin\n" ;
                            wait(NULL);
                            break;
                        }
                    }
                    if(FD_ISSET(pfds[1], &write_fds) && flag)
                    {
                        // cout << "oomadam too file dadan " << i << endl;
                        int new_portion = portion;
                        if(i<remainder)
                        {
                            new_portion++;
                        }
                        for(int k =0; k<new_portion; k++)
                        {
                            // cout << file_names[last_file] << "-" ;
                            write(pfds[1],(directory + file_names[last_file]).c_str(),32);
                            last_file++;
                        }   
                        // cout << endl; 
                        if(i!= num_of_workers-1)
                            break;
                        if(i == num_of_workers-1)
                            flag = false;
                        
                    }
                    
                }     
            }
            wait(NULL);
         
        }
        
        wait(NULL);
        
    }
}
