#include "json_defines.h"
#include "json_string.h"
#include "json_number.h"
#include "json_parser.h"

unsigned int find_bracket_pair_object(unsigned char* stream, unsigned int length_of_stream, unsigned int* end_position)
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
		else if (is_string == 1 && stream[position] == '\"')
		{
			unsigned int back_track_position = position - 1;
			unsigned int number_of_slashes = 0;
			while (stream[back_track_position] == '\\' && back_track_position != 0)
			{
				back_track_position--;
				number_of_slashes++;
			}
			if (back_track_position == 0)
			{
				return ERROR_WRONG_JSON_STRUCTURE;
			}

			if (number_of_slashes % 2 == 0)
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
	if (position == length_of_stream)
	{
		return ERROR_WRONG_JSON_STRUCTURE;
	}
	*end_position = position;
	return RETURN_OK;
}

unsigned int find_bracket_pair_array(unsigned char* stream, unsigned int length_of_stream)
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
		else if (is_string == 1 && stream[position] == '\"')
		{
			unsigned int back_track_position = position - 1;
			unsigned int number_of_slashes = 0;
			while (stream[back_track_position] == '\\' && back_track_position != 0)
			{
				back_track_position--;
				number_of_slashes++;
			}
			if (back_track_position == 0)
			{
				return ERROR_WRONG_JSON_STRUCTURE;
			}

			if (number_of_slashes % 2 == 0)
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
	while (stream[counter] <= 32 && counter < length_of_stream)
	{
		counter++;
	}
	if (counter == length_of_stream)
	{
		return NULL;
	}
	if (stream[counter] != '\"')
	{
		return NULL;
	}

	unsigned int position = counter + 1;

	while (position < length_of_stream)
	{
		if (stream[position] == '\"')
		{
			unsigned int back_track_position = position - 1;
			unsigned int number_of_slashes = 0;
			while (stream[back_track_position] == '\\' && back_track_position != 0)
			{
				back_track_position--;
				number_of_slashes++;
			}
			if (back_track_position == 0)
			{
				return NULL;
			}

			if (number_of_slashes % 2 == 0)
			{
				break;
			}
		}
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

unsigned char* parse_value(unsigned char* stream, int* value_id, unsigned int* position_value_end, unsigned int length_of_stream, unsigned int* length_of_string)
{
	unsigned int counter = 0;

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

	if ((stream[counter] >= '0' && stream[counter] <= '9') || stream[counter] == '-')
	{
		unsigned int number_of_digits = 0;
		*value_id = NUMBER;
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

	switch (stream[counter])
	{
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
		if (array_as_string == NULL)
		{
			return NULL;
		}

		memcpy(array_as_string, stream + counter, end_array_bracket + 1);
		array_as_string[end_array_bracket + 1] = '\0';
		*length_of_string = end_array_bracket + 1;
		return array_as_string;
		break;
	case 't':
		if (memcmp(stream + counter, "true", 4) == 0)
		{
			*value_id = BOOL;
			unsigned char* value = malloc(5);
			if (value == NULL)
			{
				return NULL;
			}
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
		if (memcmp(stream + counter, "false", 5) == 0)
		{
			*value_id = BOOL;
			unsigned char* value = malloc(6);
			if (value == NULL)
			{
				return NULL;
			}
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

object* parse_array(unsigned char* value_as_string_input, unsigned int length, unsigned int* number_of_values)
{
	object* obj = NULL;
	unsigned int counter = 0;
	*number_of_values = 0;
	unsigned int position_value_end = 0;
	unsigned int error_code = 0;
	unsigned char* output;
	while (1)
	{
		obj = (object*)realloc(obj, ++(*number_of_values) * sizeof(object));
		if (obj == NULL)
		{
			free(obj);
			return NULL;
		}

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
		unsigned char* value_as_string = parse_value(value_as_string_input + position_value_end + 1, &value_id, &local_position_value_end, length - position_value_end, &length_of_string);
		if (value_as_string == NULL)
		{
			return NULL;
		}

		position_value_end += local_position_value_end + 1;
		(obj[(*number_of_values) - 1]).value_identifier = value_id;

		switch (value_id)
		{
		case STRING:
			output = malloc(length_of_string + 1);
			if (output == NULL)
			{
				free(value_as_string);
				return NULL;
			}
			if (validate_string(value_as_string, output, length_of_string) != RETURN_OK)
			{
				free(value_as_string);
				return NULL;
			}
			(obj[(*number_of_values) - 1]).value_string = output;
			free(value_as_string);
			break;
		case NUMBER:
			error_code = validate_number(value_as_string, &((obj[(*number_of_values) - 1]).value_double), &((obj[(*number_of_values) - 1]).value_int), length_of_string);
			if (error_code != DOUBLE || error_code != LONG_INT)
			{
				free(value_as_string);
				return NULL;
			}

			free(value_as_string);
			break;
		case BOOL:
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
				return NULL;
			}
			break;
		case ARRAY:
			(obj[(*number_of_values) - 1]).obj = parse_array(value_as_string, length_of_string, &(obj[(*number_of_values) - 1]).number_of_values);
			if ((obj[(*number_of_values) - 1]).obj == NULL)
			{
				free(value_as_string);
				return NULL;
			}
			break;
		case OBJECT:
			if (parse_object(value_as_string, &(obj[(*number_of_values) - 1]), length, &position_value_end) != RETURN_OK)
			{
				return NULL;
			}
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
	unsigned int position_name_end;
	unsigned int value_id = 0;
	unsigned int position_value_end = 1;
	unsigned int length_of_string = 0;
	unsigned int counter = 0;
	unsigned int error_code = DEFAULT_VALUE;
	while (stream[counter] != '{' && counter < length_of_stream && stream[counter] <= 32)
	{
		counter++;
	}
	if (counter == length_of_stream || stream[counter] != '{')
	{
		return ERROR_WRONG_JSON_STRUCTURE;
	}

	unsigned int end_bracket_position;
	if (find_bracket_pair_object(stream + counter, length_of_stream - counter, &end_bracket_position) != RETURN_OK)
	{
		return ERROR_WRONG_JSON_STRUCTURE;
	}

	unsigned int is_object_empty = 1;
	for (unsigned int i = 1; i < end_bracket_position - counter; i++)
	{
		if (stream[counter + i] > 32)
		{
			is_object_empty = 0;
			break;
		}
	}

	while (1)
	{
		json_object->obj = (object*)realloc(json_object->obj, ++json_object->number_of_values * sizeof(object));
		if (json_object->obj == NULL)
		{
			free(json_object->obj);
			json_object->obj = NULL;
			return ERROR_CAN_NOT_ALLOCATE_MEMORY;
		}

		((json_object->obj)[json_object->number_of_values - 1]).obj = NULL;
		((json_object->obj)[json_object->number_of_values - 1]).name = NULL;
		((json_object->obj)[json_object->number_of_values - 1]).number_of_values = 0;
		((json_object->obj)[json_object->number_of_values - 1]).value_bool = 0;
		((json_object->obj)[json_object->number_of_values - 1]).value_double = 0.0;
		((json_object->obj)[json_object->number_of_values - 1]).value_identifier = DEFAULT_VALUE;
		((json_object->obj)[json_object->number_of_values - 1]).value_string = NULL;
		((json_object->obj)[json_object->number_of_values - 1]).value_int = 0;

		if (is_object_empty == 1)
		{
			((json_object->obj)[json_object->number_of_values - 1]).value_identifier = EMPTY_OBJECT;
		}
		else
		{
			unsigned char* name = get_string(stream + counter + 1, &position_name_end, length_of_stream - counter - 1);
			if (name == NULL)
			{
				return ERROR_FAIL_GET_NAME;
			}
			position_name_end += counter + 2;
			((json_object->obj)[json_object->number_of_values - 1]).name = name;

			while (stream[position_name_end] <= 32 && position_name_end != length_of_stream)
			{
				position_name_end++;
			}
			if ((stream[position_name_end] != ':') || (position_name_end == length_of_stream))
			{
				return ERROR_WRONG_JSON_STRUCTURE;
			}

			unsigned char* value_as_string = parse_value(stream + position_name_end + 1, &value_id, &position_value_end, length_of_stream - (position_name_end + 1), &length_of_string);
			if (value_as_string == NULL && value_id != VALUE_NULL)
			{
				return ERROR_FAIL_GET_NAME;
			}
			position_value_end += position_name_end + 1;
			((json_object->obj)[json_object->number_of_values - 1]).value_identifier = value_id;
			unsigned char* output = NULL;

			switch (value_id)
			{
			case STRING:
				output = malloc(length_of_string + 1);
				if (output == NULL)
				{
					free(value_as_string);
					return ERROR_CAN_NOT_ALLOCATE_MEMORY;
				}
				if (validate_string(value_as_string, output, length_of_string) != RETURN_OK)
				{
					free(value_as_string);
					return ERROR_WRONG_JSON_STRUCTURE;
				}
				((json_object->obj)[json_object->number_of_values - 1]).value_string = output;
				free(value_as_string);
				break;
			case NUMBER:
				error_code = validate_number(value_as_string, &(((json_object->obj)[json_object->number_of_values - 1]).value_double), &(((json_object->obj)[json_object->number_of_values - 1]).value_int), length_of_string);
				if (error_code != DOUBLE && error_code != LONG_INT)
				{
					free(value_as_string);
					return error_code;
				}

				free(value_as_string);
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
				((json_object->obj)[json_object->number_of_values - 1]).obj = parse_array(value_as_string, length_of_string, &((json_object->obj)[json_object->number_of_values - 1]).number_of_values);
				if (((json_object->obj)[json_object->number_of_values - 1]).obj == NULL)
				{
					free(value_as_string);
					return ERROR_WRONG_JSON_STRUCTURE;
				}
				break;
			case OBJECT:
				if (parse_object(value_as_string, &((json_object->obj)[json_object->number_of_values - 1]), length_of_stream - position_value_end, &position_value_end) != RETURN_OK)
				{
					return ERROR_WRONG_JSON_STRUCTURE;
				}
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
		}

		counter = position_value_end + 1;
		while (stream[counter] != '}' && stream[counter] != ',' && counter < length_of_stream)
		{
			counter++;
		}
		if (counter == length_of_stream)
		{
			return ERROR_WRONG_JSON_STRUCTURE;
		}

		if (stream[counter] == '}')
		{
			*end_pos += counter;
			return RETURN_OK;
		}
	}
	return ERROR_WRONG_JSON_STRUCTURE;
}

void free_object_memory(object* obj)
{
	if (obj->value_identifier == ARRAY || obj->value_identifier == OBJECT)
	{
		unsigned int i = 0;
		for (i = 0; i < obj->number_of_values; i++)
		{
			free_object_memory(&(obj->obj[i]));
		}
	}

	free(obj->name);
	free(obj->obj);
	free(obj->value_string);
	obj->name = NULL;
	obj->obj = NULL;
	obj->value_string = NULL;
}

unsigned int json_parse(char* fileName)
{
	object json_object;
	json_object.obj = NULL;
	json_object.number_of_values = 0;
	json_object.value_bool = 0;
	json_object.value_double = 0.0;
	json_object.value_identifier = OBJECT;
	json_object.value_string = NULL;
	json_object.value_int = 0;
	json_object.name = malloc(12);
	if (json_object.name == NULL)
	{
		return ERROR_CAN_NOT_ALLOCATE_MEMORY;
	}

	char name[] = "main_object\0";
	memcpy(json_object.name, name, 12);
	printf("JSON Parser!\n");
	unsigned char* stream;
	FILE* file;
	file = fopen(fileName, "rb");
	fseek(file, 0L, SEEK_END);
	unsigned int sz = ftell(file);
	fseek(file, 0L, SEEK_SET);
	rewind(file);
	stream = (unsigned char*)malloc((sz + 1) * sizeof(char));
	memset(stream, '\0', sz + 1);
	fread(stream, 1, sz, file);
	fclose(file);
	unsigned int position_of_last_character;
	if (parse_object(stream, &json_object, sz, &position_of_last_character) != RETURN_OK)
	{
		free_object_memory(&json_object);
		return UNDEFINED_ERROR;
	}
	free(stream);
	free_object_memory(&json_object);
	return RETURN_OK;
}