#include "json_defines.h"

/**
 * \brief checks validity of given json string
 * \param in input - unvalidated json string, without quotation marks (can be changed if necessary)
 * \param out - output buffer. Should be at least (length + 1) long (for 0 at the end).
 * \param length - length of input. If bad value, parsing returns error.
 * \return RETURN_OK if successful, other errors if validation failed
 */
int validate_string(char*, char*, int);
