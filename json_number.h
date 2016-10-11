#include "json_defines.h"

/** Takes input string which contains number
    takes two variables by address, in which it stores the result
    one is for doubles, one is for integers
    takes length of string (if different from actual strlen of input string, returns error)

    If string contained valid long integer or double value, it is returned in corresponding
    variable.
    Return value is either some error, or 100 (DOUBLE) or 101 (LONG_INT)
    if the number is double, third parameter is unused, and vice versa
    Both params should be used though, in order of not getting nullptr exception or something similar.
*/
int validate_number(char*, double*, long int*, int);
