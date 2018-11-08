#ifndef _WORKER_
#define _WORKER_
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


#define FIFO_NAME "result"
using namespace std;

class account{
public:
    account(int _id, int _dep): id(_id) , deposit(_dep) {};
    int id;
    int deposit;
};

int get_index(vector<account> accounts, int id);
void eval(string file_name);
    
#endif
