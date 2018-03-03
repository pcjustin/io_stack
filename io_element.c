#include <stdio.h>

#include "io_stack.h"
#include "io_element.h"
#include "list.h"

PIO_ELEMENT allocate_io_element(size_t input_buffer_size,
                                size_t output_buffer_size) {
	size_t malloc_size = sizeof(PIO_ELEMENT) + sizeof(int) * 2 +
	                     sizeof(size_t) * 2 +
	                     input_buffer_size +
	                     output_buffer_size;
	PIO_ELEMENT pio_element = (PIO_ELEMENT)malloc(malloc_size);

	if (!pio_element) {
		return NULL;
	}

	memset(pio_element, 0x0, malloc_size);
	pio_element->input_buffer_size = input_buffer_size;
	pio_element->output_buffer_size = output_buffer_size;
	return pio_element;
}

PIO_ELEMENT realloc_io_element(PIO_ELEMENT pio_element,
                               size_t input_buffer_size,
                               size_t output_buffer_size) {
	size_t realloc_size = sizeof(PIO_ELEMENT) + sizeof(int) * 2 +
	                      sizeof(size_t) * 2 + pio_element->input_buffer_size +
	                      pio_element->output_buffer_size + input_buffer_size +
	                      output_buffer_size;
	PIO_ELEMENT temp_io_element = (PIO_ELEMENT)realloc(pio_element, realloc_size);

	if (!temp_io_element) {
		free(pio_element);
		return NULL;
	}

	pio_element = temp_io_element;
	pio_element->input_buffer_size += input_buffer_size;
	pio_element->output_buffer_size += output_buffer_size;
	return pio_element;
}

void release_io_element(PIO_ELEMENT pio_element) {
	if (!pio_element) {
		free(pio_element);
	}
}

char* get_input_buffer(PIO_ELEMENT pio_element) {
	return pio_element->input_output_buffer;
}

char* get_output_buffer(PIO_ELEMENT pio_element) {
	return pio_element->input_output_buffer + pio_element->input_buffer_size;
}

size_t get_io_element_size(PIO_ELEMENT pio_element) {
	return sizeof(PIO_ELEMENT) + sizeof(int) * 2 + sizeof(size_t) * 2 +
	       pio_element->input_buffer_size +
	       pio_element->output_buffer_size;
}

void print_io_element(PIO_ELEMENT pio_element) {
	int input_buffer_size = pio_element->input_buffer_size;
	int output_buffer_size = pio_element->output_buffer_size;
	char* input_buffer = get_input_buffer(pio_element);
	char* output_buffer = get_input_buffer(pio_element);
	int status = pio_element->status;
	int sequence_id = pio_element->sequence_id;
	printf("status: %d\n", status);
	printf("sequence_id: 0x%x\n", sequence_id);
	printf("input_buffer_size: %zd\n", input_buffer_size);
	printf("output_buffer_size: %zd\n", output_buffer_size);

	if (input_buffer_size > 0) {
		print_buffer("input_buffer", input_buffer, input_buffer_size);
	}

	if (output_buffer_size > 0) {
		print_buffer("output_buffer", output_buffer, output_buffer_size);
	}
}

void print_buffer(const char* tag, char* buffer, size_t buffer_size) {
	printf("%s: ", tag);

	for (int i = 0; i < (int)buffer_size; i++) {
		printf("%02x ", buffer[i]);
	}

	printf("\n");
}

void print_queue(PIO_ELEMENT list) {
	printf("list_length: %zd\n", list_length(list));
	list_each(list, value) {
		int input_buffer_size = value.input_buffer_size;
		int output_buffer_size = value.output_buffer_size;
		char* input_buffer = get_input_buffer(&value);
		char* output_buffer = get_input_buffer(&value);
		printf("input_buffer_size: %zd\n", input_buffer_size);
		printf("output_buffer_size: %zd\n", output_buffer_size);

		if (input_buffer_size > 0) {
			print_buffer("input_buffer", input_buffer, input_buffer_size);
		}

		if (output_buffer_size > 0) {
			print_buffer("output_buffer", output_buffer, output_buffer_size);
		}
	}
}