#include "json_string.h"

/**
 * \brief checks validity of given json string
 * \param in input - unvalidated json string, without quotation marks (can be changed if necessary)
 * \param out - output buffer. Should be at least (length + 1) long (for 0 at the end).
 * \param length - length of input. If bad value, parsing returns error.
 * \return RETURN_OK if successful, other errors if validation failed
 */
int parse_string(char* in, char* out, int length)
{
    if (strlen(in) != length)
        return INVALID_INPUT;

    char esc = 0; //Boolean flag for escaping character
    char c;       //input buffer character
    int offset = 0; //offset created by "\" characters

    memset(out, 0, length + 1); //wipe the memory, prepare the ending zero characters

    for (int i = 0; i < length; i++)
    {
        c = in[i];

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
                case 'b' :  out[i-offset] = '\b'; esc = 0; break;
                case 'f' :  out[i-offset] = '\f'; esc = 0; break;
                case 'n' :  out[i-offset] = '\n'; esc = 0; break;
                case 'r' :  out[i-offset] = '\r'; esc = 0; break;
                case 't' :  out[i-offset] = '\t'; esc = 0; break;
                case '\\' : out[i-offset] = '\\'; esc = 0; break;
                case '\"' : out[i-offset] = '\"'; esc = 0; break;
                case '/' :  out[i-offset] = '/';  esc = 0; break;
                case 'u' :

                    offset--; //We are writing the same number of symbols as we read
                    char hexChar;
                    for (int j = 1; j < 5; j++) //checks validity of hex symbols (0-9, a-f, A-F)
                    {
                        hexChar = in[i+j];
                        if ( hexChar < '0' || hexChar > 'f'  ||
                            (hexChar > '9' && hexChar < 'A') ||
                            (hexChar > 'F' && hexChar < 'a'))
                                return INVALID_STRING;
                    }
                    out[i-offset-1] = '\\';
                    out[i-offset] = 'u';
                    esc = 0;
                    break;

                default : return INVALID_STRING;
            }
            else
                out[i-offset] = c;
        }
    }
    return RETURN_OK;
}
