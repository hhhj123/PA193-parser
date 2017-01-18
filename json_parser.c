#include "json_defines.h"
#include "json_string.h"
#include "json_number.h"
#include "json_parser.h"

#define CONTROL_CHARS 32
#define OBJ_NAME_SIZE 12

unsigned int find_bracket_pair_object(unsigned char* stream, unsigned int length_of_stream, unsigned int* end_position)
{
	unsigned int number_of_opened_brackets = 1;
	unsigned int position = 0;
	unsigned int is_string = 0;
	while (number_of_opened_brackets != 0 && position < length_of_stream)
	{
		position++;
		// Detect start of string
		if (is_string == 0 && stream[position] == '\"')
		{
			is_string = 1;
		}
		// Detect possible end of string ('"' found)
		else if (is_string == 1 && stream[position] == '\"')
		{
			unsigned int back_track_position = position - 1;
			unsigned int number_of_slashes = 0;
			// In case the character before '"' is "\"
			// check whether it is a control character or it is escaped
			while (stream[back_track_position] == '\\' && back_track_position != 0)
			{
				back_track_position--;
				number_of_slashes++;
			}

			if (number_of_slashes % 2 == 0)
			{
				is_string = 0;
			}
		}

		// Count the number of opened brackets
		switch (stream[position])
		{
		case '{':
			if (!is_string)
			{
				number_of_opened_brackets++;
			}
			break;
		case '}':
			if (!is_string)
			{
				number_of_opened_brackets--;
			}
			break;
		default:
			break;
		}
	}
	if (position == length_of_stream)
	{
		// Closing bracket not found
		return ERROR_WRONG_JSON_STRUCTURE;
	}
	*end_position = position;
	return RETURN_OK;
}

unsigned int find_bracket_pair_array(unsigned char* stream, unsigned int length_of_stream, unsigned int* end_position)
{
	unsigned int number_of_opened_brackets = 1;
	unsigned int position = 0;
	unsigned int is_string = 0;
	while (number_of_opened_brackets != 0 && position < length_of_stream)
	{
		position++;
		// Detect start of a string
		if (is_string == 0 && stream[position] == '\"')
		{
			is_string = 1;
		}
		// Detect possible end of string ('"' found)
		else if (is_string == 1 && stream[position] == '\"')
		{
			unsigned int back_track_position = position - 1;
			unsigned int number_of_slashes = 0;
			// In case the character before '"' is "\"
			// check whether it is a control character or it is escaped
			while (stream[back_track_position] == '\\' && back_track_position != 0)
			{
				back_track_position--;
				number_of_slashes++;
			}

			if (number_of_slashes % 2 == 0)
			{
				is_string = 0;
			}
		}

		// Count the number of opened brackets
		switch (stream[position])
		{
		case '[':
			if (!is_string)
			{
				number_of_opened_brackets++;
			}
			break;
		case ']':
			if (!is_string)
			{
				number_of_opened_brackets--;
			}
			break;
		default:
			break;
		}
	}
	if (position == length_of_stream)
	{
		// Closing bracket not found
		return ERROR_WRONG_JSON_STRUCTURE;
	}
	*end_position = position;
	return RETURN_OK;
}

unsigned char* get_string(unsigned char* stream, int* position_name_end, unsigned int length_of_stream)
{
	unsigned int counter = 0;
	// Skip control characters
	while (stream[counter] <= CONTROL_CHARS && counter < length_of_stream)
	{
		counter++;
	}
	if (counter == length_of_stream)
	{
		return NULL;
	}
	// Verify that the string starts with '"'
	if (stream[counter] != '\"')
	{
		return NULL;
	}

	unsigned int position = counter + 1;

	while (position < length_of_stream)
	{
		// Detect possible end of string ('"' found)
		if (stream[position] == '\"')
		{
			unsigned int back_track_position = position - 1;
			unsigned int number_of_slashes = 0;
			// In case the character before '"' is "\"
			// check whether it is a control character or it is escaped
			while (stream[back_track_position] == '\\' && back_track_position != 0)
			{
				back_track_position--;
				number_of_slashes++;
			}

			if (number_of_slashes % 2 == 0)
			{
				break;
			}
		}
		position++;
	}

	// Closing '"' not found
	if (position == length_of_stream)
	{
		return NULL;
	}

	// Allocate memory for the string
	unsigned char* name = malloc(position - counter);
	if (name == NULL)
	{
		return NULL;
	}

	// Copy the string to the allocated memory
	memcpy(name, stream + counter + 1, position - counter - 1);
	name[position - counter - 1] = '\0';
	*position_name_end = position;

	return name;
}

unsigned char* parse_value(unsigned char* stream, int* value_id, unsigned int* position_value_end, unsigned int length_of_stream, unsigned int* length_of_string)
{
	unsigned int counter = 0;

	// Skip control characters
	while (stream[counter] <= CONTROL_CHARS && counter < length_of_stream)
	{
		counter++;
	}
	if (counter == length_of_stream)
	{
		return NULL;
	}

	// Check if the value starts with a valid character
	if (stream[counter] != '\"' &&
		stream[counter] != '{' &&
		stream[counter] != '[' &&
		stream[counter] != 't' &&
		stream[counter] != 'f' &&
		stream[counter] != 'n' &&
		stream[counter] != '-' &&
		((stream[counter] < '0' || stream[counter] > '9'))
		)
	{
		return NULL;
	}

	// Detect a number
	if ((stream[counter] >= '0' && stream[counter] <= '9') || stream[counter] == '-')
	{
		unsigned int number_of_digits = 0;
		*value_id = NUMBER;
		// Count the number of digits
		while (((stream[counter + number_of_digits] >= '0' && stream[counter + number_of_digits] <= '9') || stream[counter + number_of_digits] == '-'
			|| stream[counter + number_of_digits] == '+'
			|| stream[counter + number_of_digits] == '.'
			|| stream[counter + number_of_digits] == 'E'
			|| stream[counter + number_of_digits] == 'e') && counter + number_of_digits < length_of_stream)
		{
			number_of_digits++;
		}
		if (counter + number_of_digits == length_of_stream)
		{
			return NULL;
		}
		// Allocate memory and copy the number there
		unsigned char* number = malloc(number_of_digits + 1);
		if (number == NULL)
		{
			return NULL;
		}
		memcpy(number, stream + counter, number_of_digits);
		number[number_of_digits] = '\0';
		*length_of_string = number_of_digits;
		*position_value_end = counter + number_of_digits - 1;
		return number;
	}

	// Differentiate between other types of values
	// Set the end position
	switch (stream[counter])
	{
	// Detect a string
	case '\"':
		*value_id = STRING;
		unsigned char* value_string = get_string(stream + counter, position_value_end, length_of_stream - counter);
		if (value_string == NULL)
		{
			return NULL;
		}
		*position_value_end += counter;
		*length_of_string = *position_value_end - counter - 1;

		return value_string;
		break;
	// Detect an object
	case '{':
		*value_id = OBJECT;
		*position_value_end = counter;
		return stream + counter;
		break;
	// Detect an array
	case '[':
		*value_id = ARRAY;
		unsigned int end_array_bracket;
		if (find_bracket_pair_array(stream + counter, length_of_stream - counter, &end_array_bracket) != RETURN_OK)
		{
			return NULL;
		}
		*position_value_end = counter + end_array_bracket;

		// Allocate memory and copy the array there
		unsigned char* array_as_string = malloc(end_array_bracket + 2);
		if (array_as_string == NULL)
		{
			return NULL;
		}

		memcpy(array_as_string, stream + counter, end_array_bracket + 1);
		array_as_string[end_array_bracket + 1] = '\0';
		*length_of_string = end_array_bracket + 1;
		return array_as_string;
		break;
	// Detect a boolean value (true)
	case 't':
		if (memcmp(stream + counter, "true", 4) == 0)
		{
			*value_id = BOOL;
			// Allocate memory for "true"
			unsigned char* value = malloc(5);
			if (value == NULL)
			{
				return NULL;
			}
			memcpy(value, "true", 4);
			value[4] = '\0';
			*position_value_end = counter + 3;
			return value;
		}
		else
		{
			return NULL;
		}
		break;
	// Detect a boolean value (false)
	case 'f':
		if (memcmp(stream + counter, "false", 5) == 0)
		{
			*value_id = BOOL;
			// Allocate memory for "false"
			unsigned char* value = malloc(6);
			if (value == NULL)
			{
				return NULL;
			}
			memcpy(value, "false", 5);
			value[5] = '\0';
			*position_value_end = counter + 4;
			return value;
		}
		else
		{
			return NULL;
		}
		break;
	// Detect null value
	case 'n':
		if (!memcmp(stream + counter, "null", 4))
		{
			*value_id = VALUE_NULL;
			*position_value_end = counter + 3;
			return NULL;
		}
		else
		{
			return NULL;
		}
		break;
	default:
		break;
	}

	return NULL;
}

object* parse_array(unsigned char* value_as_string_input, unsigned int length, unsigned int* number_of_values, int* is_array_empty)
{
	object* obj = NULL;
	unsigned int counter = 0;
	*number_of_values = 0;
	unsigned int position_value_end = 0;
	unsigned int error_code = 0;
	unsigned char* output;

	// Check if the array is empty (do not consider control characters)
	for (unsigned int i = 1; i < length; i++)
	{
		if (value_as_string_input[i] > CONTROL_CHARS && i == length - 1)
		{
			*is_array_empty = 1;
			return NULL;
		}
		if (value_as_string_input[i] > CONTROL_CHARS)
		{
			break;
		}
	}

	while (1)
	{
		object* tmp_memory = (object*)realloc(obj, ++(*number_of_values) * sizeof(object));
		if (tmp_memory == NULL)
		{
			(*number_of_values)--;
			free_array_memory(obj, *number_of_values);
			return NULL;
		}
		obj = tmp_memory;

		// Initialize the sub-object (an element of the array)
		(obj[(*number_of_values) - 1]).obj = NULL;
		(obj[(*number_of_values) - 1]).name = NULL;
		(obj[(*number_of_values) - 1]).number_of_values = 0;
		(obj[(*number_of_values) - 1]).value_bool = 0;
		(obj[(*number_of_values) - 1]).value_double = 0.0;
		(obj[(*number_of_values) - 1]).value_identifier = 0;
		(obj[(*number_of_values) - 1]).value_string = NULL;
		(obj[(*number_of_values) - 1]).value_int = 0;

		unsigned int value_id = 0;
		unsigned int length_of_string = 0;
		unsigned int local_position_value_end = 0;

		// Get the value as a string and learn its type
		unsigned char* value_as_string = parse_value(value_as_string_input + position_value_end + 1, &value_id, &local_position_value_end, length - position_value_end, &length_of_string);
		if (value_as_string == NULL && value_id != VALUE_NULL)
		{			
			free_array_memory(obj, *number_of_values);
			return NULL;
		}

		position_value_end += local_position_value_end + 1;
		(obj[(*number_of_values) - 1]).value_identifier = value_id;
		int is_array_empty = 0;

		// Process the value depending on its type
		switch (value_id)
		{
		case STRING:
			output = malloc(length_of_string + 1);
			if (output == NULL)
			{
				free_array_memory(obj, *number_of_values);
				free(value_as_string);
				return NULL;
			}
			// Validate the string
			if (validate_string(value_as_string, output, length_of_string) != RETURN_OK)
			{
				free_array_memory(obj, *number_of_values);
				free(value_as_string);
				return NULL;
			}
			// If the string is correct copy it to the string value
			// of the currently proccessed element of the array
			// and free the original string
			(obj[(*number_of_values) - 1]).value_string = output;
			free(value_as_string);
			break;
		case NUMBER:
			// Validate the number
			error_code = validate_number(value_as_string, &((obj[(*number_of_values) - 1]).value_double), &((obj[(*number_of_values) - 1]).value_int), length_of_string);
			if (error_code != DOUBLE || error_code != LONG_INT)
			{
				free_array_memory(obj, *number_of_values);
				free(value_as_string);
				return NULL;
			}

			free(value_as_string);
			break;
		case BOOL:
			// Assign "true" or "false" to the bool value of the array element
			if (memcmp(value_as_string, "true", 4) == 0)
			{
				(obj[(*number_of_values) - 1]).value_bool = TRUE;
			}
			else if (memcmp(value_as_string, "false", 5) == 0)
			{
				(obj[(*number_of_values) - 1]).value_bool = FALSE;
			}
			else
			{
				free_array_memory(obj, *number_of_values);
				return NULL;
			}
			break;
		case ARRAY:
			// Parse the array
			(obj[(*number_of_values) - 1]).obj = parse_array(value_as_string, length_of_string, &(obj[(*number_of_values) - 1]).number_of_values, &is_array_empty);
			if ((obj[(*number_of_values) - 1]).obj == NULL)
			{
				// Check emptiness of the array
				if (is_array_empty == 1)
				{
					(obj[(*number_of_values) - 1]).value_identifier = EMPTY_ARRAY;
					(obj[(*number_of_values) - 1]).number_of_values = 0;
				}
				else
				{
					free(value_as_string);
					(obj[(*number_of_values) - 1]).number_of_values = 0;
					free_array_memory(obj, *number_of_values);
					return NULL;
				}
			}
			break;
		case OBJECT:
			// Parse the object
			if (parse_object(value_as_string, &(obj[(*number_of_values) - 1]), length, &position_value_end) != RETURN_OK)
			{
				free_array_memory(obj, *number_of_values);
				return NULL;
			}
			break;
		case VALUE_NULL:
			if (value_as_string != NULL)
			{
				free_array_memory(obj, *number_of_values);
				return NULL;
			}
			break;
		default:
			break;
		}

		counter = position_value_end + 1;
		// Skip control characters
		while (value_as_string_input[counter] <= CONTROL_CHARS && counter < length)
		{
			counter++;
		}
		// ']' or ',' not found, the array format is incorrect
		if (counter == length && value_as_string_input[counter] != ']' && value_as_string_input[counter] != ',')
		{
			free_array_memory(obj, *number_of_values);
			return NULL;
		}
		position_value_end = counter;
		// The end of the array reached
		if (value_as_string_input[counter] == ']')
		{
			return obj;
		}
	}
}

unsigned int parse_object(unsigned char* stream, object* json_object, unsigned int length_of_stream, unsigned int* end_pos)
{
	json_object->obj = NULL;
	unsigned int position_name_end;
	unsigned int value_id = 0;
	unsigned int position_value_end = 1;
	unsigned int length_of_string = 0;
	unsigned int counter = 0;
	unsigned int error_code = DEFAULT_VALUE;
	
	// Skip control chars and reach the first opening bracket
	while (stream[counter] != '{' && counter < length_of_stream && stream[counter] <= CONTROL_CHARS)
	{
		counter++;
	}
	
	// The first opening bracket not found
	if (counter == length_of_stream || stream[counter] != '{')
	{
		return ERROR_WRONG_JSON_STRUCTURE;
	}

	unsigned int end_bracket_position;
	// Pair closing bracket not found
	if (find_bracket_pair_object(stream + counter, length_of_stream - counter, &end_bracket_position) != RETURN_OK)
	{
		return ERROR_WRONG_JSON_STRUCTURE;
	}

	// Check if the object is empty
	unsigned int is_object_empty = 1;
	for (unsigned int i = 1; i < end_bracket_position - counter; i++)
	{
		if (stream[counter + i] > CONTROL_CHARS)
		{
			is_object_empty = 0;
			break;
		}
	}
	
	if (is_object_empty == 1)
	{
		json_object->value_identifier = EMPTY_OBJECT;
		json_object->number_of_values = 0;
		*end_pos += end_bracket_position;
		
		return RETURN_OK;
	}

	while (1)
	{
		object* tmp_memory = (object*)realloc(json_object->obj, ++json_object->number_of_values * sizeof(object));
		if (tmp_memory == NULL)
		{
			json_object->number_of_values--;
			return ERROR_CAN_NOT_ALLOCATE_MEMORY;
		}

		json_object->obj = tmp_memory;

		// Initialize the sub-object
		((json_object->obj)[json_object->number_of_values - 1]).obj = NULL;
		((json_object->obj)[json_object->number_of_values - 1]).name = NULL;
		((json_object->obj)[json_object->number_of_values - 1]).number_of_values = 0;
		((json_object->obj)[json_object->number_of_values - 1]).value_bool = 0;
		((json_object->obj)[json_object->number_of_values - 1]).value_double = 0.0;
		((json_object->obj)[json_object->number_of_values - 1]).value_identifier = DEFAULT_VALUE;
		((json_object->obj)[json_object->number_of_values - 1]).value_string = NULL;
		((json_object->obj)[json_object->number_of_values - 1]).value_int = 0;	
		
		// Get the name of the object
		unsigned char* name = get_string(stream + counter + 1, &position_name_end, length_of_stream - counter - 1);
		if (name == NULL)
		{
			return ERROR_FAIL_GET_NAME;
		}
		position_name_end += counter + 2;
		((json_object->obj)[json_object->number_of_values - 1]).name = name;

		while (stream[position_name_end] <= CONTROL_CHARS && position_name_end != length_of_stream)
		{
			position_name_end++;
		}

		if ((stream[position_name_end] != ':') || (position_name_end == length_of_stream))
		{
			return ERROR_WRONG_JSON_STRUCTURE;
		}

		// Get the value of the object as a string and learn its type	
		unsigned char* value_as_string = parse_value(stream + position_name_end + 1, &value_id, &position_value_end, length_of_stream - (position_name_end + 1), &length_of_string);
		if (value_as_string == NULL && value_id != VALUE_NULL)
		{
			return ERROR_WRONG_JSON_STRUCTURE;
		}

		position_value_end += position_name_end + 1;
		((json_object->obj)[json_object->number_of_values - 1]).value_identifier = value_id;
		unsigned char* output = NULL;
		int is_array_empty = 0;

		// Process the value depending on its type
		switch (value_id)
		{
		case STRING:
			output = malloc(length_of_string + 1);
			if (output == NULL)
			{
				free(value_as_string);
				return ERROR_CAN_NOT_ALLOCATE_MEMORY;
			}
			// Validate the string
			if (validate_string(value_as_string, output, length_of_string) != RETURN_OK)
			{
				free(value_as_string);
				return ERROR_WRONG_JSON_STRUCTURE;
			}
			// In case the string is valid copy it to the object's string value
			((json_object->obj)[json_object->number_of_values - 1]).value_string = output;
			free(value_as_string);
			break;
		case NUMBER:
			// Validate the number
			error_code = validate_number(value_as_string, &(((json_object->obj)[json_object->number_of_values - 1]).value_double), &(((json_object->obj)[json_object->number_of_values - 1]).value_int), length_of_string);
			if (error_code != DOUBLE && error_code != LONG_INT)
			{
				free(value_as_string);
				return error_code;
			}

			free(value_as_string);
			break;
		case BOOL:
			// Assign "true" or "false" to the object's bool value
			if (memcmp(value_as_string, "true", 4) == 0)
			{
				((json_object->obj)[json_object->number_of_values - 1]).value_bool = TRUE;
			}
			else if (memcmp(value_as_string, "false", 5) == 0)
			{
				((json_object->obj)[json_object->number_of_values - 1]).value_bool = FALSE;
			}
			else
			{
				free(value_as_string);
				return ERROR_WRONG_JSON_STRUCTURE;
			}
			free(value_as_string);
			break;
		case ARRAY:	
			// Parse the array			
			((json_object->obj)[json_object->number_of_values - 1]).obj = parse_array(value_as_string, length_of_string, &((json_object->obj)[json_object->number_of_values - 1]).number_of_values, &is_array_empty);
			if (((json_object->obj)[json_object->number_of_values - 1]).obj == NULL)
			{
				// Check emptiness of the array
				if (is_array_empty == 1)
				{
					((json_object->obj)[json_object->number_of_values - 1]).value_identifier = EMPTY_ARRAY;
					((json_object->obj)[json_object->number_of_values - 1]).number_of_values = 0;
				}
				else
				{
					free(value_as_string);
					((json_object->obj)[json_object->number_of_values - 1]).number_of_values = 0;
					return ERROR_WRONG_JSON_STRUCTURE;
				}
			}
				

			free(value_as_string);
			break;
		case OBJECT:
			// Parse the object
			if (parse_object(value_as_string, &((json_object->obj)[json_object->number_of_values - 1]), length_of_stream - position_value_end, &position_value_end) != RETURN_OK)
			{					
				return ERROR_WRONG_JSON_STRUCTURE;
			}				
			break;
		case VALUE_NULL:
			if (value_as_string != NULL)
			{
				free(value_as_string);
				return ERROR_WRONG_JSON_STRUCTURE;
			}
			break;
		default:
			break;
		}		

		counter = position_value_end + 1;
		// Reach the end of the current sub-object (',') or the whole object ('}')
		while (stream[counter] != '}' && stream[counter] != ',' && counter < length_of_stream)
		{
			counter++;
		}
		if (counter == length_of_stream)
		{
			// Closing bracket '}' or objects separator ',' not found
			return ERROR_WRONG_JSON_STRUCTURE;
		}

		// The end of the object reached
		if (stream[counter] == '}')
		{
			*end_pos += counter;
			return RETURN_OK;
		}
	}
	return ERROR_WRONG_JSON_STRUCTURE;
}

void free_array_memory(object* obj, int number_of_objects)
{
	// Free all objects of the array
	for (int i = 0; i < number_of_objects; i++)
	{
		free_object_memory(&obj[i]);
	}
}

void free_object_memory(object* obj)
{
	// Recursively free all values of an array of an object
	if (obj->value_identifier == ARRAY || obj->value_identifier == OBJECT)
	{
		unsigned int i = 0;
		for (i = 0; i < obj->number_of_values; i++)
		{
			if (&(obj->obj[i]) != NULL)
			{
				free_object_memory(&(obj->obj[i]));
			}
		}
	}

	// Free name, value_string and object
	free(obj->name);
	free(obj->obj);
	free(obj->value_string);
	obj->name = NULL;
	obj->obj = NULL;
	obj->value_string = NULL;
}

unsigned int json_parse(char* fileName)
{
	// Initialize the main object
	object json_object;
	json_object.obj = NULL;
	json_object.number_of_values = 0;
	json_object.value_bool = 0;
	json_object.value_double = 0.0;
	json_object.value_identifier = OBJECT;
	json_object.value_string = NULL;
	json_object.value_int = 0;
	json_object.name = malloc(OBJ_NAME_SIZE);
	if (json_object.name == NULL)
	{
		return ERROR_CAN_NOT_ALLOCATE_MEMORY;
	}

	char name[] = "main_object\0";
	memcpy(json_object.name, name, OBJ_NAME_SIZE);
	printf("JSON Parser!\n");
	
	// Open the .json file for reading
	FILE* file;
	fopen_s(&file, fileName, "rb");
	if (file == NULL)
	{
		free(json_object.name);
		return ERROR_CAN_NOT_OPEN_FILE;
	}

	// Find out the size of the data
	unsigned char data[2];
	data[1] = '\0';
	unsigned int data_counter = 0;
	short possible_overflow = 0;
	while (!feof(file))
	{
		if (data_counter == UINT_MAX)
		{
			possible_overflow = 1;
			break;
		}
		fread(data, 1, 1, file);
		data_counter++;
	}
	if (possible_overflow == 1)
	{
		fclose(file);
		free_object_memory(&json_object);

		return UNDEFINED_ERROR;
	}

	fseek(file, 0L, SEEK_END);
	unsigned int sz = ftell(file);
	fseek(file, 0L, SEEK_SET);
	rewind(file);

	// Allocate memory for the data
	unsigned char* stream = (unsigned char*)malloc((sz + 1) * sizeof(char));
	if (stream == NULL)
	{
		fclose(file);
		free_object_memory(&json_object);
		return ERROR_CAN_NOT_ALLOCATE_MEMORY;
	}
	memset(stream, '\0', sz + 1);

	// Read the data from the file
	fread(stream, 1, sz, file);
	fclose(file);
	unsigned int position_of_last_character;

	// Parse the main object
	if (parse_object(stream, &json_object, sz, &position_of_last_character) != RETURN_OK)
	{
		free_object_memory(&json_object);
		free(stream);
		return UNDEFINED_ERROR;
	}
	
	// Free memory
	free(stream);
	free_object_memory(&json_object);

	return RETURN_OK;
}
