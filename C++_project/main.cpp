#include <iostream>
#include <string>
#include <ctime>

#include "CRM.hpp"


using namespace std;


int main()
{

    string logfile_path = "./logfile_CRM";
    CRM crm(logfile_path);

    return 0;
}