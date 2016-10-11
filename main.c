#include "json_defines.h"
#include "json_string.h"
#include "json_number.h"
//TODO FREE MEMORY WHEN ERROR OCCURS

typedef struct object object;
struct object
{
	unsigned int number_of_values;
	int value_identifier;
	unsigned char* name;
	unsigned char* value_string;
	float value_float;
	long value_int;
	short value_bool;
	object* obj;
};

unsigned int parse_object(unsigned char*, object*, unsigned int, unsigned int*);

unsigned int find_bracket_pair_object(unsigned char* stream, unsigned int length_of_stream)
{
	unsigned int number_of_opened_brackets = 1;
	unsigned int position = 0;
	unsigned int is_string = 0;
	while (number_of_opened_brackets != 0 && position < length_of_stream)
	{
		position++;
		if (is_string == 0 && stream[position] == '\"')
		{
			is_string = 1;
		}

		if (is_string == 1 && stream[position] == '\"')
		{
			if (stream[position - 1] != '\\')
			{
				is_string = 0;
			}
		}

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
	return position;
}

unsigned int find_bracket_pair_array(unsigned char* stream, int length_of_stream)
{
	unsigned int number_of_opened_brackets = 1;
	unsigned int position = 0;
	unsigned int is_string = 0;
	while (number_of_opened_brackets != 0 && position < length_of_stream)
	{
		position++;
		if (is_string == 0 && stream[position] == '\"')
		{
			is_string = 1;
		}

		if (is_string == 1 && stream[position] == '\"')
		{
			if (stream[position - 1] != '\\')
			{
				is_string = 0;
			}
		}

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
	return position;
}

unsigned char* get_string(unsigned char* stream, int* position_name_end, unsigned int length_of_stream)
{
	unsigned int counter = 0;
	while (stream[counter] != '\"' && counter < length_of_stream)
	{
		counter++;
	}
	if (counter == length_of_stream)
	{
		return NULL;
	}

	unsigned int position = counter + 1;
	while ((stream[position] != '\"' || stream[position - 1] == '\\') && position < length_of_stream)
	{
		position++;
	}
	if (position == length_of_stream)
	{
		return NULL;
	}

	unsigned char* name = malloc(position - counter);
	if (name == NULL)
	{
		return NULL;
	}

	memcpy(name, stream + counter + 1, position - counter - 1);
	name[position - counter - 1] = '\0';
	*position_name_end = position;

	return name;
}

unsigned char* parse_value(unsigned char* stream, int* value_id, unsigned int* position_value_end, unsigned int length_of_stream, unsigned int* length_of_string) //todo add parameter length of stream
{
	unsigned int counter = 0;
	unsigned int is_number = 0;
	while (stream[counter] <= 32 && counter < length_of_stream)
	{
		counter++;
	}
	if (counter == length_of_stream)
	{
		return NULL;
	}

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

	if ((stream[counter] > '0' && stream[counter] <= '9') || stream[counter] == '-')
	{
		is_number = 1;
	}

	if (is_number == 1)//todo parse number
	{
		unsigned int number_of_digits = 0;
		while (stream[counter + number_of_digits] > '0' && stream[counter + number_of_digits] <= '9')
		{
			number_of_digits++;
		}
		unsigned char* number = malloc(number_of_digits + 1);
		memcpy(number, stream + counter, number_of_digits);
		number[number_of_digits] = '\0';
		*value_id = NUMBER;
		*position_value_end = counter + number_of_digits;
		return number;
	}

	switch (stream[counter])
	{
		case '\"':
			*value_id = STRING;
			unsigned char* value_string = get_string(stream + counter, position_value_end, length_of_stream - counter);
			*position_value_end += counter;
			return value_string;
			break;
		case '{':
			*value_id = OBJECT;
			*position_value_end = counter;
			return stream + counter;
			break;
		case '[':
			*value_id = ARRAY;
			unsigned int end_array_bracket = find_bracket_pair_array(stream + counter, length_of_stream - counter);
			*position_value_end = counter + end_array_bracket;
			unsigned char* array_as_string = malloc(end_array_bracket + 2);
			memcpy(array_as_string, stream + counter, end_array_bracket + 1);
			array_as_string[end_array_bracket + 1] = '\0';
			*length_of_string = end_array_bracket + 1;
			return array_as_string;
			break;
		case 't':
			//todo check if length of string in memcmp would not overflow
			if (memcmp(stream + counter, "true", 4) == 0)
			{
				*value_id = BOOL;
				unsigned char* value = malloc(5);
				memcpy(value, "true", 4);
				value[4] = '\0';
				*position_value_end = counter + 4;
				return value;
			}
			else
			{
				return NULL;
			}
			break;
		case 'f':
			//todo check if length of string in memcmp would not overflow
			if (memcmp(stream + counter, "false", 5) == 0)
			{
				*value_id = BOOL;
				unsigned char* value = malloc(6);
				memcpy(value, "false", 5);
				value[5] = '\0';
				*position_value_end = counter + 5;
				return value;
			}
			else
			{
				return NULL;
			}
			break;
		case 'n':
			//todo check if length of string in memcmp would not overflow
			if (!memcmp(stream + counter, "null", 4))
			{
				*value_id = VALUE_NULL;
				*position_value_end = counter + 4;
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

object* parse_array(unsigned char* value_as_string_input, unsigned int length)
{
	object* obj = NULL;
	unsigned int counter = 0;
	unsigned int number_of_values = 0;
	unsigned int position_value_end = 0;
	while (1)
	{
		obj = (object*)realloc(obj, ++number_of_values * sizeof(object));
		if (obj == NULL)
		{
			free(obj);
			return NULL;
		}

		(obj[number_of_values - 1]).obj = NULL;
		(obj[number_of_values - 1]).number_of_values = 0;
		(obj[number_of_values - 1]).value_bool = 0;
		(obj[number_of_values - 1]).value_float = 0.0;
		(obj[number_of_values - 1]).value_identifier = 0;
		(obj[number_of_values - 1]).value_string = NULL;
		(obj[number_of_values - 1]).value_int = 0;

		unsigned int value_id = 0;
		unsigned int length_of_string = 0;
		unsigned int local_position_value_end = 0;
		unsigned char* value_as_string = parse_value(value_as_string_input + position_value_end + 1, &value_id, &local_position_value_end, length - position_value_end, &length_of_string);
		if (value_as_string == NULL)
		{
			return NULL;
		}
		position_value_end += local_position_value_end;
		(obj[number_of_values - 1]).value_identifier = value_id;

		switch (value_id)
		{
		case STRING:
			(obj[number_of_values - 1]).value_string = value_as_string;
			break;
		case NUMBER:
			(obj[number_of_values - 1]).value_string = value_as_string;
			break;
		case BOOL:
			if (memcmp(value_as_string, "true", 4) == 0)
			{
				(obj[number_of_values - 1]).value_bool = TRUE;
			}
			else if (memcmp(value_as_string, "false", 5) == 0)
			{
				(obj[number_of_values - 1]).value_bool = FALSE;
			}
			break;
		case ARRAY:
			(obj[number_of_values - 1]).obj = parse_array(value_as_string, length_of_string);
			break;
		case OBJECT:
			parse_object(value_as_string, &(obj[number_of_values - 1]), length, &position_value_end);
			break;
		case VALUE_NULL:
			if (value_as_string != NULL)
			{
				return NULL;
			}
			break;
		default:
			break;
		}

		counter = position_value_end + 1;
		while (value_as_string_input[counter] != ']' && value_as_string_input[counter] != ',' && counter < length)
		{
			counter++;
		}
		if (counter == length)
		{
			return NULL;
		}
		position_value_end = counter;
		if (value_as_string_input[counter] == ']')
		{
			return obj;
		}
	}
}

unsigned int parse_object(unsigned char* stream, object* json_object, unsigned int length_of_stream, unsigned int* end_pos)
{
	json_object->obj = NULL;
	unsigned int counter = 0;
	while (stream[counter] != '{' && counter < length_of_stream && stream[counter] <= 32)
	{
		counter++;
	}
	if (counter == length_of_stream)
	{
		return ERROR_WRONG_JSON_STRUCTURE;
	}

	unsigned int end_bracket_position = find_bracket_pair_object(stream + counter, length_of_stream - counter);
	while (1)
	{
		json_object->obj = (object*)realloc(json_object->obj, ++json_object->number_of_values * sizeof(object));
		if (json_object->obj == NULL)
		{
			free(json_object->obj);
			return ERROR_CAN_NOT_ALLOCATE_MEMORY;
		}

		((json_object->obj)[json_object->number_of_values - 1]).obj = NULL;
		((json_object->obj)[json_object->number_of_values - 1]).number_of_values = 0;
		((json_object->obj)[json_object->number_of_values - 1]).value_bool = 0;
		((json_object->obj)[json_object->number_of_values - 1]).value_float = 0.0;
		((json_object->obj)[json_object->number_of_values - 1]).value_identifier = DEFAULT_VALUE;
		((json_object->obj)[json_object->number_of_values - 1]).value_string = NULL;
		((json_object->obj)[json_object->number_of_values - 1]).value_int = 0;
		//todo object can be empty
		unsigned int position_name_end;
		unsigned char* name = get_string(stream + counter + 1, &position_name_end, length_of_stream - counter - 1);
		if (name == NULL)
		{
			return ERROR_FAIL_GET_NAME;
		}
		position_name_end += counter + 2;
		((json_object->obj)[json_object->number_of_values -1]).name = name;

		while (stream[position_name_end] <= 32)
		{
			position_name_end++;
		}
		if (stream[position_name_end] != ':')
		{
			return ERROR_WRONG_JSON_STRUCTURE;
		}

		unsigned int value_id = 0;
		unsigned int position_value_end;
		unsigned int length_of_string = 0;
		unsigned char* value_as_string = parse_value(stream + position_name_end + 1, &value_id, &position_value_end, length_of_stream - (position_name_end + 1), &length_of_string);
		if (value_as_string == NULL && value_id != VALUE_NULL)
		{
			return ERROR_FAIL_GET_NAME;
		}
		position_value_end += position_name_end;
		((json_object->obj)[json_object->number_of_values - 1]).value_identifier = value_id;
		unsigned char* ouput = NULL;//output from string validation function
		switch (value_id)
		{
		case STRING:
			//add matej's function
			//input == value_as_string
			//length
			((json_object->obj)[json_object->number_of_values - 1]).value_string = value_as_string;
			break;
		case NUMBER:
			//todo add function matej's number validator
			((json_object->obj)[json_object->number_of_values - 1]).value_string = value_as_string;
			break;
		case BOOL:
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
				return ERROR_WRONG_JSON_STRUCTURE;
			}
			break;
		case ARRAY:
			((json_object->obj)[json_object->number_of_values - 1]).obj = parse_array(value_as_string, length_of_string);//todo check error codes
			break;
		case OBJECT:
			parse_object(value_as_string, &((json_object->obj)[json_object->number_of_values - 1]), length_of_stream, &position_value_end); //todo check error codes
			break;
		case VALUE_NULL:
			if (value_as_string != NULL)
			{
				return ERROR_WRONG_JSON_STRUCTURE;
			}
			break;
		default:
			break;
		}
		//add control
		counter = position_value_end + 1;
		while (stream[counter] != '}' && stream[counter] != ',' && counter < length_of_stream)
		{
			counter++;
		}
		if (stream[counter] == '}')
		{
			*end_pos += counter + 1;
			return;
		}
	}
}

int main()
{
	object json_object;
	json_object.obj = NULL;
	json_object.number_of_values = 0;
	json_object.value_bool = 0;
	json_object.value_float = 0.0;
	json_object.value_identifier = DEFAULT_VALUE;
	json_object.value_string = NULL;
	json_object.value_int = 0;
	json_object.name = malloc(12);
	char name[] = "main_object\0";
	memcpy(json_object.name, name, 12);
    printf("JSON Parser!\n");
	unsigned char* stream;
	FILE* file;
	fopen_s(&file, "jsonExample.txt", "rb");
	fseek(file, 0L, SEEK_END);
	unsigned int sz = ftell(file); //todo integer can overflow when file has more bytes then size of unsigned int
	fseek(file, 0L, SEEK_SET);
	rewind(file);
	stream = (unsigned char*)malloc((sz + 1) * sizeof(char));
	memset(stream, '\0', sz + 1);
	fread_s(stream, sz, 1, sz, file);
	unsigned int position_of_last_character;
	parse_object(stream, &json_object, sz, &position_of_last_character);
	printf("%s", stream);
	free(stream);
	fclose(file);
	getchar();

    return 0;
}
