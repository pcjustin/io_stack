#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "io_element.h"
#include "io_stack.h"
#include "list.h"

#define BUFFER_SIZE PIPE_BUF

void send_back(PIO_STACK pio_stack, PIO_ELEMENT pio_element, size_t pio_element_size) {
	char* input_buffer = get_input_buffer(pio_element);
	char* output_buffer = get_output_buffer(pio_element);
	PIO_ELEMENT send_io_element = allocate_io_element(pio_element->input_buffer_size,
	                              pio_element->input_buffer_size);
	size_t send_io_element_size = get_io_element_size(send_io_element);
	send_io_element->status = pio_element->status;
	send_io_element->sequence_id = pio_element->sequence_id;
	send_io_element->input_buffer_size = pio_element->input_buffer_size;
	send_io_element->output_buffer_size = pio_element->input_buffer_size;
	char* send_input_buffer = get_input_buffer(send_io_element);
	char* send_output_buffer = get_output_buffer(send_io_element);
	memcpy(send_input_buffer, input_buffer, send_io_element->input_buffer_size);
	memcpy(send_output_buffer, output_buffer, send_io_element->output_buffer_size);
	send_buffer_no_wait(pio_stack, send_io_element);
	print_io_element(send_io_element);
	release_io_element(send_io_element);
}

void do_something(void* arg) {
	PIO_STACK pio_stack = (PIO_STACK)arg;

	if (!pio_stack) {
		return;
	}

	char buffer[BUFFER_SIZE];

	for (;;) {
		memset(buffer, 0x0, BUFFER_SIZE);
		size_t buffer_size = receive_last_buffer(pio_stack, buffer);

		if (buffer_size == 0) {
			sleep(1);
			continue;
		}

		send_back(pio_stack, (PIO_ELEMENT)buffer, buffer_size);
	}
}

int main(int argc, char** argv) {
	printf("server is running\n");
	PIO_STACK pio_stack = create_io_stack(SERVER_WRITE);
	pthread_t thread;
	pthread_create(&thread, NULL, (void*)do_something, (void*)pio_stack);
	run_io_stack(pio_stack);
	destroy_io_stack(pio_stack);
	return 0;
}
