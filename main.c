#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETURN_OK         0
#define INVALID_INPUT     1
#define INVALID_STRING    2

char parsed[10000]; //temporary solution

/**
 * Takes parsed string with/without (needs to be decided) quotation marks and checks validity of string inside,
 * then saves it if ok.
 */
int parse_string(char* str, int length)
{

    char esc = 0; //Boolean flag for escaping character
    char c;       //input buffer character
    int offset = 0; //offset created by "\" characters

    if (strlen(str) != length)
        return INVALID_INPUT;

    for (int i = 0; i < length; i++)
    {
        c = str[i];

        if (c == '\\' && !esc)
        {
            esc = 1;
            offset++;
        }
        else
        {
            if (esc)
            switch(c)
            {   // ALl cases of special symbols in strings
                case 'b' :  parsed[i-offset] = '\b'; esc = 0; break;
                case 'f' :  parsed[i-offset] = '\f'; esc = 0; break;
                case 'n' :  parsed[i-offset] = '\n'; esc = 0; break;
                case 'r' :  parsed[i-offset] = '\r'; esc = 0; break;
                case 't' :  parsed[i-offset] = '\t'; esc = 0; break;
                case '\\' : parsed[i-offset] = '\\'; esc = 0; break;
                case '\"' : parsed[i-offset] = '\"'; esc = 0; break;
                case '/' :  parsed[i-offset] = '/';  esc = 0; break;
                case 'u' :

                    offset--; //We are writing the same number of symbols as we read
                    char hexChar;
                    for (int j = 1; j < 5; j++) //checks validity of hex symbols (0-9, a-f, A-F)
                    {
                        hexChar = str[i+j];
                        if ( hexChar < '0' || hexChar > 'f'  ||
                            (hexChar > '9' && hexChar < 'A') ||
                            (hexChar > 'F' && hexChar < 'a'))
                                return INVALID_STRING;
                    }
                    parsed[i-offset-1] = '\\';
                    parsed[i-offset] = 'u';
                    esc = 0;
                    break;

                default : return INVALID_STRING;
            }
            else
                parsed[i-offset] = c;
        }
    }

    return RETURN_OK;
}

int main(int argc, char * argv[])
{
    if (argc != 2);
    int ret =parse_string(argv[1], strlen(argv[1]));
    printf("%s", parsed);
    return ret;
}
