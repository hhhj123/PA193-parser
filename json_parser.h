#include "json_defines.h"
#include "json_string.h"
#include "json_number.h"


/** \brief Structure in which we store the whole .json file, recursively. */
typedef struct object object;
struct object
{
	unsigned int number_of_values;  // number of values in object, array, etc.
	int value_identifier;           // distinguishes the value type stored in object
	unsigned char* name;            // contains the name of object
	unsigned char* value_string;    // if the value is string, it is stored here
	double value_double;            // if the value is double, it is stored here
	long int value_int;             // if the value is integer, it is stored here
	short value_bool;               // if the value is boolean, it is stored here
	object* obj;                    // pointer to array of sub-objects or arrays (tree structure)
};

/**
 * @brief main function of parser, recursively reads and stores all entities in .json file
 * @param stream stream of what are we parsing
 * @param json_object where to store parsed .json
 * @param length_of_stream total length of parsed part of stream
 * @param end_pos saved ending position in the part of stream
 */
unsigned int parse_object(unsigned char*, object*, unsigned int, unsigned int*);

/**
 * @brief read the array and store it in the structure
 * @param value_string_as_input input is in form of string, later is converted
 * @param length the length of string containing array
 * @param number_of_values number of values stored in array
 */
object* parse_array(unsigned char*, unsigned int, unsigned int*);

/**
 * @brief takes the string and selects which type of value is stored there
 * @param stream input stream
 * @param value_id stores the type of value parsed
 * @param position_value_end sets the pointer to the end of stream
 * @param length_of_stream length of input
 * @param length_of_string length of output
 */
unsigned char* parse_value(unsigned char*, int* value_id, unsigned int*, unsigned int, unsigned int*);

/**
 * @brief takes out everything between double quotes
 * @param stream input stream
 * @param position_name_end sets the pointer to the end of stream
 * @param length_of_stream length of input
 */
unsigned char* get_string(unsigned char*, int*, unsigned int);

/**
 * @brief finds pair of brackets of array [ ]
 * @param stream input stream
 * @param length of stream total length of input stream
 */
unsigned int find_bracket_pair_array(unsigned char*, unsigned int);

/**
 * @brief finds pair of brackets of object { }
 * @param stream input stream
 * @param length_of_stream total length of input stream
 * @param end_position sets pointer to the end
 */
unsigned int find_bracket_pair_object(unsigned char*, unsigned int, unsigned int*);

/**
 * @brief frees the memory to clean up
 * @param obj root object in the structure
 */
void free_object_memory(object*);
