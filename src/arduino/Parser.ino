//------------------------------------------------------------------------------------------------------------------
#include "Parser.h"
//------------------------------------------------------------------------------------------------------------------
bool Parser::parse(String line)
{
    argc = 0;
    memset(buffer, 0, sizeof(buffer));
    memset(argv, 0, sizeof(char*) * PARSER_MAX_PARAMS);
    strcat(buffer, line.c_str());

    int idx = 0;
    while (idx < line.length())
    {
       int start = idx;
       while (idx < line.length() && buffer[idx] != ' ') idx++;
       if (start != idx)
       {
           argv[argc++] = &buffer[start];
           buffer[idx++] = 0;
       }
        while (idx < line.length() && buffer[idx] == ' ') idx++;
    }

    if (argc >= 1 && argc <= PARSER_MAX_PARAMS)
       return true; 
    else    
       return false;
}
//------------------------------------------------------------------------------------------------------------------
int Parser::getArgc(void)
{
    return argc;
}
//------------------------------------------------------------------------------------------------------------------
char** Parser::getArgv(void)
{
    return argv;
}
//------------------------------------------------------------------------------------------------------------------
