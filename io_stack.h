#ifndef __IO_STACK_H__
#define __IO_STACK_H__

#include "io_element.h"

typedef struct _IO_STACK {
	int status;
	int client_fd;
	int server_fd;
} IO_STACK, *PIO_STACK;

PIO_STACK allocate_io_stack(void);
void destroy_io_stack(PIO_STACK pio_stack);

PIO_STACK create_io_stack(int flag);
void run_io_stack(PIO_STACK pio_stack);
void stop_io_stack(void);

int send_buffer_no_wait(PIO_STACK pio_stack, PIO_ELEMENT input_element);
size_t receive_last_buffer(PIO_STACK pio_stack, void* receive_buffer);
size_t receive_buffer(PIO_STACK pio_stack, int sequence_id, void* receive_buffer);

#endif