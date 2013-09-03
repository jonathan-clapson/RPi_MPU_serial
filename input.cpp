#include <cstdio>
#include <pthread.h>
#include <cstring>

#include "input.h"

void* get_string( void *ptr )
{
	struct read_message_type *r_msg = (struct read_message_type *) ptr;	
	while (r_msg->end_thread == 0) {
		if (! (r_msg->message_valid) ) {
			scanf("%200s", (char *) r_msg->message);
			strcat(r_msg->message, "\n");
			r_msg->message_valid = 1;
		}
	}
}

void spawn_input_thread(pthread_t *thread, struct read_message_type *r_msg)
{	
	r_msg->message_valid = 0;
	r_msg->end_thread = 0;
	
	pthread_create( thread, NULL, get_string, (void*) r_msg );
}

void end_input_thread(pthread_t *thread, struct read_message_type *r_msg)
{
	r_msg->end_thread = 1;
	pthread_join ( *thread, NULL);
}
