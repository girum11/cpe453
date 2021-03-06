#include <pthread.h>
#include "util.h"

#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#define NUM_PHILOSOPHERS 5

typedef enum {
	EATING, THINKING, CHANGING
} State;

typedef struct {
	int id; /* The ID of this Philosopher */

	pthread_t thread; /* The pthread_t used to run Philosopher
	 state logic. */

	int des_left_fork; /* Integer representing which fork the
	 Philosopher will be holding in his left hand. Don't change this. */
	int des_right_fork; /* Integer representing which fork the
	 Philosopher will be holding in his right hand.  Don't change this. */

	State state; /* Enum for the State that the Philosopher is currently
	 in (EATING, THINKING, or CHANGING) */
} Philosopher;

typedef struct {
	int id; /* The ID of this Fork. */

	Philosopher *owner; /* A pointer to the Philosopher currently holding
	 this Fork, or NULL if none. */

	pthread_mutex_t mutex_lock; /* Mutex lock for this Fork. */
} Fork;

Philosopher phils[NUM_PHILOSOPHERS]; /* The Philosophers. */
Fork forks[NUM_PHILOSOPHERS]; /* The Forks. */

pthread_mutex_t global_mutex_lock; /* A global mutex thread responsible
 for ensuring Philosopher state output is clean. */

void change_state(Philosopher *philosopher, State new_state);
void pick_up_fork(Philosopher *philosopher, Fork *fork);
void put_down_fork(Philosopher *philosopher, Fork *fork);
void dawdle(Philosopher *philosopher);

#endif
