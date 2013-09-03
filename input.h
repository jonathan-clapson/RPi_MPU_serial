#ifndef __INPUT_H__
#define __INPUT_H__

#include <pthread.h>

#define message_size 200

struct read_message_type {
	int message_valid;
	int end_thread;
	char message[message_size];
};

void spawn_input_thread(pthread_t *thread, struct read_message_type *r_msg);
void end_input_thread(pthread_t *thread, struct read_message_type *r_msg);

#endif /* __INPUT_H__ */
