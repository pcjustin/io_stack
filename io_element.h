#ifndef __IO_ELEMENT_H__
#define __IO_ELEMENT_H__

#define CLIENT_WRITE 1
#define SERVER_READ  2
#define CLIENT_READ  3
#define SERVER_WRITE 4

typedef struct _IO_ELEMENT {
	int status;
	int sequence_id;
	size_t input_buffer_size;
	size_t output_buffer_size;
	char input_output_buffer[1];
} IO_ELEMENT, *PIO_ELEMENT;

PIO_ELEMENT allocate_io_element(size_t input_buffer_size, size_t output_buffer_size);
void release_io_element(PIO_ELEMENT pio_element);
size_t get_io_element_size(PIO_ELEMENT pio_element);

char* get_input_buffer(PIO_ELEMENT pio_element);
char* get_output_buffer(PIO_ELEMENT pio_element);

PIO_ELEMENT add_io_element_to_queue(PIO_ELEMENT list, PIO_ELEMENT pio_element);

void print_io_element(PIO_ELEMENT pio_element);
void print_buffer(const char* tag, char* buffer, size_t buffer_size);
void print_queue(PIO_ELEMENT list);

#endif