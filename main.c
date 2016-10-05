#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_string.h"

//example main for json_string.c
int main(int argc, char * argv[])
{
    if (argc != 2)
        return INVALID_INPUT;

    char* parsed = malloc(strlen(argv[1]) + 1);

    int ret = parse_string(argv[1], parsed, strlen(argv[1]));
    printf("%s", parsed);
    free(parsed);
    return ret;
}
