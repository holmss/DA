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

typedef struct  _right
{
    std::string tok;
    int dist;
} right;

std::vector<word> dictionary;

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
        std::cerr << "Can`t create or open file " << stats_file <<", please try again\n";
        exit(CANT_OPEN_FILE);
    }

    //parsing input file: done----------------------------------------------------

    std::string buf;

    getline(f_inp,buf);
    while (!f_inp.eof())
    {
        getline(f_inp,buf);

        char * s = new char[buf.size() + 1];

        strcpy(s, buf.c_str());

        char * tmp = strtok(s, "\t");

        std::vector<std::string> lexem_container; //a full string received from the file
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

int levenshtein (const std::string source, const std::string target)
{
    if(source.length() > target.length())
        return levenshtein(target, source);

    const int min_size = source.length(), max_size = target.length();
    std::vector<int> distance(min_size + 1);

    for(int i = 0; i <= min_size; ++i)
        distance[i] = i;

    for(int j = 1; j <= max_size; ++j)
    {
        int prev = distance[0], prev_save;
        ++distance[0];

        for(int i = 1; i <= min_size; ++i)
        {
            prev_save = distance[i];

            if(source[i - 1] == target[j - 1])
                distance[i] = prev;

            else
                distance[i] = std::min(std::min(distance[i - 1], distance[i]), prev) + 1;

            prev = prev_save;
        }
    }

    return distance[min_size];
}

std::string viterbi(std::vector<std::vector<right>> correct_req)
{
    std::string correct_string = "";

    bool found = false;
    
    word correct_word;
    correct_word.count = 0;

    if(correct_req.size() == 1)
    {
        for(int i = 0; i < correct_req[0].size(); ++i)
            for(word w : dictionary)
                if(w.first == correct_req[0][i].tok && w.count > correct_word.count)
                {
                    correct_word.first = w.first;
                    correct_word.count = w.count;
                    found = true;
                }

        return correct_word.first;
    }

    for(int i = 0; i < correct_req[0].size(); ++i)
    {
        correct_word.count = 0;
        for(int j = 0; j < correct_req[1].size(); ++j)
            for(word w : dictionary)
            {
                if(w.first == correct_req[0][i].tok && w.second == correct_req[1][j].tok && w.count > correct_word.count)
                {
                    correct_word.first = w.first;
                    correct_word.second = w.second;
                    correct_word.count = w.count;
                    found = true;
                }
            }
    }

    if(!found)
    {
        for(std::vector<right> r : correct_req)
            correct_string = correct_string + " " + r[0].tok;

        return correct_string;
    }

    correct_string = correct_string + " " + correct_word.first + " " + correct_word.second;
    std::string tmp = correct_word.second;

    for(int i = 2; i < correct_req.size(); ++i)
    {
        correct_word.count = 0;
        for(int j = 0; j < correct_req[i].size(); ++j)
            for(word w : dictionary)
            {
                if(tmp == w.first && correct_req[i][j].tok == w.second && w.count > correct_word.count)
                {
                    correct_word.second = w.second;
                    correct_word.count = w.count;
                }
            }
        tmp = correct_word.second;
        correct_string = correct_string + " " + correct_word.second;
    }

    return correct_string;
}

void correct(std::string stats_file, std::string inp_file, std::string out_file)
{
    //checking files: done--------------------------------------------------------

    bool stdinp = false;

    std::ifstream f_inp;
    f_inp.open(inp_file.c_str(), std::ios_base::in);
    
    if(!f_inp)
    {
        std::cerr << "Can`t open file, make sure file " << inp_file << " exists and try again\n";
        exit(CANT_OPEN_FILE);
    }

    std::ifstream f_stats;
    f_stats.open(stats_file.c_str(), std::ios_base::in);

    if(!f_stats)
    {
        std::cerr << "Can`t open file, make sure file " << stats_file << " exists and try again\n";
        exit(CANT_OPEN_FILE);
    }

    std::ofstream f_out;
    f_out.open(out_file.c_str(), std::ios_base::out);

    if(!f_out)
    {
        std::cerr << "Can`t create or open file " << out_file <<", please try again\n";
        exit(CANT_OPEN_FILE);
    }

    //parsing the dictionary: done------------------------------------------------

    std::string buf;

    while(!f_stats.eof())
    {
        getline(f_stats, buf);

        if (buf == "")
            break;

        char * s = new char[buf.size() + 1];

        strcpy(s, buf.c_str());

        char * tmp = strtok(s, "\t");

        std::vector<std::string> lexem_container;

        while(tmp)
        {
            lexem_container.push_back(tmp);
            tmp = strtok(NULL, "\t");
        }

        word wrd;

        wrd.first = lexem_container[0];

        if(lexem_container.size() == 3)
        {
            wrd.second = lexem_container[1];
            wrd.count = std::stoi(lexem_container[2]);
        }
        else
        {
            wrd.second = "";
            wrd.count = std::stoi(lexem_container[1]);
        }

        dictionary.push_back(wrd);
    }

    //correcting mistakes: done---------------------------------------------------

    while(!f_inp.eof())
    {
        getline(f_inp, buf);
        std::cout << buf << "\n\n";

        std::istringstream ist(buf);

        std::vector<right> right_words;
        std::vector<std::vector<right>> right_request;

        std::string token;

        right rt;

        //making vector of possible words for one request:

        while(ist >> token)
        {  
            for(word w : dictionary)
            {
                rt.tok = w.first;
                rt.dist = levenshtein(token, w.first);

                if(right_words.empty())
                    right_words.push_back(rt);

                else if(rt.dist < right_words[0].dist)
                {
                    right_words.clear();
                    right_words.push_back(rt);
                }

                else if(rt.dist == right_words[0].dist && rt.tok != right_words[0].tok)
                    right_words.push_back(rt);
            }

            right_request.push_back(right_words);

            right_words.clear();
        }

        //trying to make right request:

        std::cout << "Maybe you mean: " << viterbi(right_request) << "\n\n";
    }
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