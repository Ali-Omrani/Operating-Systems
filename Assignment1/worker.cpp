#include "worker.h"

int get_index(vector<account> accounts, int id)
{
    for(int i=0; i<accounts.size(); i++)
    {
        if(accounts[i].id == id)
            return i;
    }
    return -1;
}

void eval(string file_name)
{
    vector<account> accounts;
    mknod(FIFO_NAME, S_IFIFO | 0666, 0);
    int fd;
    fd = open(FIFO_NAME, O_WRONLY);
    string line;
    ifstream myfile (file_name);
    string result = file_name;
    result += ": ";
    cout << "eval ----> file_name =" << file_name << "-" << endl;
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            // cout << line << '\n';
            stringstream mystream(line);
            int id1;
            mystream >> id1;
            //cout << "id1 =" << id1 << endl;
            string identifier;
            getline(mystream, identifier, ' ' );
            getline(mystream, identifier, ' ' );
            //cout << "identifier =" << identifier << endl;
            if(identifier == ":")
            {
                int deposit;
                mystream >> deposit;
                account new_account(id1, deposit);
                accounts.push_back(new_account);
                //cout << "new account with id1 =" << id1 << " deposit =" << deposit << endl; 
            }
            else
            {
                
                //cout << "transaction" << endl;
                int id2;
                mystream >> id2;
                
                int index1, index2;
                index1 = get_index(accounts, id1);
                index2 = get_index(accounts, id2);

                getline(mystream, identifier, ' ' );
                getline(mystream, identifier, ' ' );
                //cout << "identifier" << identifier << endl;
                int amount;
                mystream >> amount;
                //cout << "amount =" << amount << endl;
                accounts[index1].deposit-=amount;
                accounts[index2].deposit+=amount;

                cout << "new deposits:" <<  accounts[index1].deposit << " " <<  accounts[index2].deposit << endl;
              
                if(accounts[index1].deposit < 0)
                {
                    result+= to_string(accounts[index1].id) +  " fraud detected"; 
                    cout << result << endl; 
                    int num;
                    if((num = write(fd, result.c_str(), result.length())) == -1)
                    perror("worker: write ");
                    // else
                    // cout << "written " << num << "bytes :" << result << endl;            
                }
                   
            }


        }
        


        myfile.close();
       // close(fd);
    }
    
    else cout << "Unable to open file"; 
}
