#include "json_number.h"

int validate_number(char* in, double* outd, long int* outl, int length)
{
    if (strlen(in) != length || strlen(in) == 0)
        return INVALID_INPUT;

    long int outLong;
    double outDouble;
    int returnType;

    char flag_double = 0;

    int i = 0;
    while (i < length)
    {
        //check if all characters are valid, and if it should be parsed as long int or double
        switch(in[i])
        {
            case '0' :
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' :
            case '9' :
            case '-' :
            case '+' : i++; break;
            case '.' :
            case 'e' :
            case 'E' : flag_double = 1; i++; break;
            default: return INVALID_NUMBER;
        }
    }

    errno = 0; //error handling

    if (!flag_double) //strtol is enough
    {
        returnType = LONG_INT;
        outLong = strtol(in, NULL, 10);
        //strtol return 0 if can't convert.
        //Shouldn't happen, since we checked if only valid characters are there.

        if (errno == ERANGE)
            return INVALID_NUMBER;

        *outl = outLong;
    }

    if (flag_double == 1) //strtod needs to be used
    {
        returnType = DOUBLE;
        outDouble = strtod(in, NULL);
        //strtod returns 0.0 if can't convert.
        //Shouldn't happen, since we checked if only valid characters are there.

        if (errno == ERANGE)
            return INVALID_NUMBER;

        *outd = outDouble;
    }

    return returnType;
}
