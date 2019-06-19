#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "cloudlet.h"

using namespace std;

void printUsage(const char *b)
{
    printf("\n");
    printf("Usage: %s [options]\n", b);
    printf("Program Options:\n");
    printf("  -n  <string>    Name of Database Table\n");
    printf("  -c  <string>    Write config file with <string>\n");
    printf("  -h  help        Print usage information\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    // Parse commandline options
    int options;
    std::string dbName;
    Cloudlet cloud;
    while ( (options = getopt(argc, argv, "n:c:t:o:h")) != -1 )
    { // for each option...
        switch (options)
        {
        case 'n'://Database Table Name
            dbName = optarg;
            break;
        case 'c'://Write Database Config file
            dbName = optarg;
            printf("Making Database Config File with Name: %s\n",optarg);
            Utils::makeDBConfigFile(dbName);
            break;
        case 't'://Create hazard table
            cloud.createHazardTable();
            printf("Making Hazard Table: HAZARDS\n");
            break;
        case 'o'://create Notify statement
            cloud.createNotify();
            printf("Made Notify statement\n");
            break;
        case 'h'://Help
            printUsage(argv[0]);
            break;
        default: //Default to Help
            printUsage(argv[0]);
            break;
        }
    }

    // print usage if no argument given!
    
    if (optind > argc)
    {
        printUsage(argv[0]);
        return 1;
    }
    
}