#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#define ARGS_ERROR -1
#define CANT_OPEN_FILE -2

//----------------------------------------------------------------------------

typedef struct _word 
{
    std::string first;
    std::string second;
    int count;
} word;

void usage()
{
    //man: done-------------------------------------------------------------------

    std::cerr << "usage error detected:\n to learn run: \n" 
    << "\t ./prog learn --input <input file> --output <stats file> \n"
    << "to correct run: \n" 
    << "\t ./prog correct --stats <stats file> --input <input file> --output <output file> \n";
}

void learn(std::string inp_file, std::string stats_file)
{
    //checking files: done--------------------------------------------------------

    std::cout << "learning ███▒▒▒▒▒▒▒\n\n";
    
    std::ifstream f_inp;
    f_inp.open(inp_file.c_str(), std::ios_base::in);
    
    if(!f_inp)
    {
        std::cerr << "Can`t open file, make sure file " << inp_file << " exists and try again\n";
        exit(CANT_OPEN_FILE);
    }

    std::ofstream f_stats;
    f_stats.open(stats_file.c_str(), std::ios_base::out);

    if(!f_stats)
    {
        std::cerr << "Can`t create or open file, please try another name\n";
        exit(CANT_OPEN_FILE);
    }

    //parsing input file: done----------------------------------------------------
    //memory leak!!!

    std::string buf;
    std::vector<word> dictionary;

    getline(f_inp,buf);
    while (!f_inp.eof())
    {
        getline(f_inp,buf);

        char * s = new char[buf.size() + 1];

        strcpy(s, buf.c_str());

        char * tmp = strtok(s, "\t");

        std::vector<std::string> lexem_container;
        std::vector<std::string> request; //splited request - separate words from request

        while(tmp)
        {
            lexem_container.push_back(tmp);
            tmp = strtok(NULL, "\t");
        }

        buf = lexem_container[1];

        std::istringstream ist(buf);
        std::string token;

        while (ist >> token)
            request.push_back(token);

        for (int i = 0; i < request.size(); i++)
        {
            word wd;
            wd.count = 1;

            wd.first = request[i];
            if (i != request.size()-1)
                wd.second = request[i+1];
            else
                wd.second = "";

            int j = 0;

            for (j = 0; j < dictionary.size(); j++)
            {
                if (dictionary[j].first == wd.first && dictionary[j].second == wd.second)
                {
                    dictionary[j].count++;
                    break;
                }
            }  

            if(j == dictionary.size())
                dictionary.push_back(wd);  
        }
    }

    for (word w : dictionary)
    {
        std::cout << "first: " << w.first 
        << "\t second: " << w.second
        << "\t count: " << w.count << "\n";
    }

    //writing to file: done-------------------------------------------------------


    for (word w : dictionary)
    {
        f_stats <<  w.first << "\t"
        << w.second << "\t"
        << w.count << "\n";
    }

    std::cout << "\ndone ✔\n";
}

void correct(std::string stats_file, std::string inp_file, std::string out_file)
{
    //checking files: in progress-------------------------------------------------
    std::cout << "correcting...\n";
    // std::cout << stats_file << " " << inp_file << " " << out_file << "\n";

    //correcting mistakes: in progress--------------------------------------------
}


void mktest(std::string source)
{
    learn(source, "stats.txt");

    //make test file from stats file: in progress---------------------------------
}

int main(int argc, char * argv[])
{
    //reading from stream: done---------------------------------------------------

    std::string inp_file, stats_file, out_file;
    bool inp = false, stats = false, out = false;

    // std::cout << argc << "\n";

    if(argc < 2)
    {
        usage();
        return ARGS_ERROR;
    }
    if(strcmp(argv[1], "learn") == 0)
    {
        if(argc != 6)
        {
            usage();
            return ARGS_ERROR;
        }

        for (int i = 2; i < argc; i++)
        {
            if(strcmp(argv[i], "--input") == 0)
            {
                inp = true;
                inp_file = argv[i+1];
            }

            if(strcmp(argv[i], "--output") == 0)
            {
                stats = true;
                stats_file = argv[i+1];
            }
        }
    

        if(!inp || !stats)
        {
            usage();
            return ARGS_ERROR;
        }

        learn(inp_file, stats_file);
    }

    else if(strcmp(argv[1], "correct") == 0)
    {
        if(argc != 8)
        {
            usage();
            return ARGS_ERROR;
        }

        for (int i = 2; i < argc; i++)
        {
            if(strcmp(argv[i], "--stats") == 0)
            {
                stats = true;    
                stats_file = argv[i+1];
            }

            if(strcmp(argv[i], "--input") == 0)
            {
                inp = true;
                inp_file = argv[i+1];
            }

            if(strcmp(argv[i], "--output") == 0)
            {
                out = true;
                out_file = argv[i+1];
            }
        }

        if(!inp || !stats || !out)
        {
            usage();
            return ARGS_ERROR;
        }

        correct(stats_file, inp_file, out_file);
    }

    else if (strcmp(argv[1], "mktest") == 0)
        mktest(argv[2]);

    else 
    { 
        usage();
        return ARGS_ERROR;
    }

    return 0;
}