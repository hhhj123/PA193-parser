#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define RETURN_OK         0

#define DOUBLE            100
#define LONG_INT          101

#define DEFAULT_VALUE 0
#define STRING 1
#define FLOAT 2
#define INTEGER 3
#define BOOL 4
#define TRUE 5
#define FALSE 6
#define ARRAY 7
#define OBJECT 8
#define VALUE_NULL 9
#define NUMBER 10

#define ERROR_INTERNAL 11
#define ERROR_CAN_NOT_OPEN_FILE 12
#define ERROR_CAN_NOT_ALLOCATE_MEMORY 13
#define ERROR_WRONG_JSON_STRUCTURE 14
#define ERROR_FAIL_GET_NAME 15
#define ERROR_FAIL_GET_VALUE 16

#define INVALID_INPUT     17
#define INVALID_STRING    18
#define INVALID_NUMBER    19
#define INVALID_VALUE     20
#define UNDEFINED_ERROR   255
