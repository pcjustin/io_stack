/**
 * @brief Provide a simple code to describe the functional
 * 
 * @file client.c
 * @author Justin Lu (pcjustin)
 * @date 2018-03-03
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>

#include "io_stack.h"
#include "io_element.h"

void do_something(void* arg) {
	PIO_STACK pio_stack = (PIO_STACK)arg;

	if (!pio_stack) {
		return;
	}

	for (;;) {
		char buffer[BUFFER_SIZE];
		memset(buffer, 0x0, BUFFER_SIZE);
		printf("input string: ");
		scanf("%s", buffer);
		size_t buffer_size = strlen(buffer);
		PIO_ELEMENT pio_element = allocate_io_element(buffer_size, 0);
		size_t pio_element_size = get_io_element_size(pio_element);
		char* input_buffer = get_input_buffer(pio_element);
		memcpy(input_buffer, buffer, buffer_size);
		int sequence_id = send_buffer_no_wait(pio_stack, pio_element);
		release_io_element(pio_element);
		memset(buffer, 0x0, BUFFER_SIZE);

		while ((buffer_size = receive_buffer(sequence_id, buffer)) == 0);

		print_io_element((PIO_ELEMENT)buffer);
	}
}

int main(int argc, char** argv) {
	PIO_STACK pio_stack = create_io_stack(CLIENT_WRITE);
	pthread_t thread = NULL;
	pthread_create(&thread, NULL, (void*)do_something, (void*)pio_stack);
	run_io_stack(pio_stack);
	destroy_io_stack(pio_stack);
	return 0;
}