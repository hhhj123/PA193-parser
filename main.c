#include "json_parser.h"

int main()
{
    int ret = json_parse("test01.json");

    if(!ret)
        printf("PARSING SUCCESSFUL !\n");
    else
        printf("Problem occured!\n");
	getchar();

	return ret;
}
