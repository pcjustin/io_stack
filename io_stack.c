/**
 * @brief Implement a I/O Stack with named pipe
 *
 * @file io_stack.c
 * @author Justin Lu (pcjustin)
 * @date 2018-03-03
 */
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "io_stack.h"
#include "list.h"

static list(IO_ELEMENT, list);
static int __io_terminate = 0;
static void sig_handler(int signo) {
	__io_terminate = 1;
}

static void register_signal(void) {
	struct sigaction act;
	act.sa_handler = &sig_handler;
	sigaction(SIGINT, &act, NULL);
}

PIO_STACK allocate_io_stack(void) {
	size_t malloc_size = sizeof(PIO_STACK) + sizeof(int) * 2;
	PIO_STACK pio_stack = (PIO_STACK)malloc(malloc_size);

	if (!pio_stack) {
		return NULL;
	}

	memset(pio_stack, 0x0, malloc_size);
	return pio_stack;
}

PIO_STACK create_io_stack(int flag) {
	register_signal();
	PIO_STACK pio_stack = allocate_io_stack();

	if (!pio_stack) {
		return NULL;
	}

	int client_fd = 0, server_fd = 0;
	umask(0);

	if (flag == CLIENT_WRITE) {
		client_fd = open(CLIENT_FIFO, O_RDWR);

		if (client_fd == -1) {
			free(pio_stack);
			return NULL;
		}

		server_fd = open(SERVER_FIFO, O_RDWR);

		if (server_fd == -1) {
			free(pio_stack);
			return NULL;
		}
	} else if (flag == SERVER_WRITE) {
		int ret = access(CLIENT_FIFO, F_OK);

		if (ret == -1) {
			ret = mkfifo(CLIENT_FIFO, 0666);

			if (ret == -1) {
				return NULL;
			}
		}

		ret = access(SERVER_FIFO, F_OK);

		if (ret == -1) {
			ret = mkfifo(SERVER_FIFO, 0666);

			if (ret == -1) {
				return NULL;
			}
		}

		server_fd = open(SERVER_FIFO, O_RDWR);

		if (server_fd == -1) {
			free(pio_stack);
			return NULL;
		}

		client_fd = open(CLIENT_FIFO, O_RDWR);

		if (client_fd == -1) {
			free(pio_stack);
			return NULL;
		}
	}

	pio_stack->status = flag;
	pio_stack->client_fd = client_fd;
	pio_stack->server_fd = server_fd;
	return pio_stack;
}

void run_io_stack(PIO_STACK pio_stack) {
	int client_fd = pio_stack->client_fd;
	int server_fd = pio_stack->server_fd;

	for (;;) {
		fd_set fds;
		int maxfd = 0;
		int buffer_size = 0;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0x0, BUFFER_SIZE);
		FD_ZERO(&fds);
		FD_SET(client_fd, &fds);
		FD_SET(server_fd, &fds);
		struct timeval tv = {1, 0};
		maxfd = (client_fd > server_fd) ? client_fd : server_fd;

		if (select(maxfd + 1, &fds, NULL, NULL, &tv) < 0) {
			perror("select");
		} else {
			if (__io_terminate) {
				break;
			}

			if (pio_stack->status == SERVER_WRITE && FD_ISSET(client_fd, &fds)) {
				buffer_size = read(client_fd, &buffer, BUFFER_SIZE);

				if (buffer_size > 0) {
					PIO_ELEMENT pio_element = (PIO_ELEMENT)malloc(buffer_size);
					memcpy(pio_element, buffer, buffer_size);
					list_push(list, *pio_element);
					release_io_element(pio_element);
				}
			}

			if (pio_stack->status == CLIENT_WRITE && FD_ISSET(server_fd, &fds)) {
				buffer_size = read(server_fd, &buffer, BUFFER_SIZE);

				if (buffer_size > 0) {
					PIO_ELEMENT pio_element = (PIO_ELEMENT)malloc(buffer_size);
					memcpy((void*)pio_element, (void*)buffer, buffer_size);
					list_push(list, *pio_element);
					release_io_element(pio_element);
				}
			}
		}
	}
}

void stop_io_stack(void) {
	__io_terminate = 1;
}

void destroy_io_stack(PIO_STACK pio_stack) {
	int client_fd = pio_stack->client_fd;
	int server_fd = pio_stack->server_fd;
	int status = pio_stack->status;
	int ret = close(client_fd);

	if (ret == -1) {
		perror("close client_fd");
	}

	ret = close(server_fd);

	if (ret == -1) {
		perror("close server_fd");
	}

	if (status == SERVER_WRITE) {
		ret = unlink(CLIENT_FIFO);

		if (ret == -1) {
			perror("unlink fifo");
		}

		ret = unlink(SERVER_FIFO);

		if (ret == -1) {
			perror("unlink fifo");
		}
	}

	list_clear(list);

	if (pio_stack) {
		free(pio_stack);
	}
}

size_t receive_last_buffer(PIO_STACK pio_stack, void* receive_buffer) {
	size_t pio_element_size = 0;

	if (list_length(list) == 0) {
		return 0;
	}

	list_each_elem(list, elem) {
		PIO_ELEMENT pio_element = (PIO_ELEMENT)elem;

		if ((pio_stack->status == CLIENT_WRITE && pio_element->status == SERVER_WRITE) ||
		    (pio_stack->status == SERVER_WRITE && pio_element->status == CLIENT_WRITE)) {
			pio_element_size = get_io_element_size(pio_element);
			memcpy(receive_buffer, pio_element, pio_element_size);
			list_elem_remove(elem);
			break;
		}

		break;
	}
	return pio_element_size;
}

size_t receive_buffer(int sequence_id, void* receive_buffer) {
	size_t pio_element_size = 0;

	if (list_length(list) == 0) {
		return 0;
	}

	list_each_elem(list, elem) {
		PIO_ELEMENT pio_element = (PIO_ELEMENT)elem;

		if (pio_element->sequence_id == sequence_id) {
			pio_element_size = get_io_element_size(pio_element);
			memcpy(receive_buffer, pio_element, pio_element_size);
			list_elem_remove(elem);
			break;
		}
	}
	return pio_element_size;
}

int send_buffer_no_wait(PIO_STACK pio_stack, PIO_ELEMENT input_element) {
	int client_fd = pio_stack->client_fd;
	int server_fd = pio_stack->server_fd;
	int status = pio_stack->status;
	input_element->status = status;

	if (status == CLIENT_WRITE) {
		srand(time(NULL));
		int sequence_id = rand() & 0xFFFF;
		input_element->sequence_id = sequence_id;
		size_t input_element_size = get_io_element_size(input_element);
		write(client_fd, input_element, input_element_size);
	} else if (status == SERVER_WRITE) {
		size_t input_element_size = get_io_element_size(input_element);
		write(server_fd, input_element, input_element_size);
	}

	return input_element->sequence_id;
}
