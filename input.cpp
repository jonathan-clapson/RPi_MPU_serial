#include <cstdio>
#include <pthread.h>
#include <cstring>

#include "input.h"

/*
 * grab input from console
 * this function is run from a second thread
 */
void* get_string( void *ptr )
{
	struct read_message_type *r_msg = (struct read_message_type *) ptr;	
	while (r_msg->end_thread == 0) {
		if (!r_msg->message_valid) {
			r_msg->receiving = false;
			
			//it would be great to use gets here, but i want to pause console output while you type things in :S
			//gets(r_msg->message);
			for (int i=0; i<sizeof(r_msg->message); i++)
			{
				r_msg->message[i] = getc(stdin);
				printf("char: %c\n", r_msg->message[i]);
				r_msg->receiving = true;

				if (r_msg->message[i] == '\n')
					break;
			}
			
			r_msg->message_valid = 1;
		}
	}
}

void spawn_input_thread(pthread_t *thread, struct read_message_type *r_msg)
{	
	r_msg->message_valid = false;
	r_msg->end_thread = false;
	
	pthread_create( thread, NULL, get_string, (void*) r_msg );
}

void end_input_thread(pthread_t *thread, struct read_message_type *r_msg)
{
	r_msg->end_thread = true;
	pthread_join ( *thread, NULL);
}
