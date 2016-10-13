#include "json_parser.h"

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		return 1;
	}

    int ret = json_parse(argv[1]);

    if(!ret)
        printf("PARSING SUCCESSFUL !\n");
    else
        printf("Problem occured!\n");
	getchar();

	return ret;
}
