#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

typedef struct
{
    int id;             /* The philosopher's id: 0 to 5 */
    long t0;            /* The time when the program started */
    long ms;            /* The maximum time that philosopher sleeps/eats */
    void *v;            /* The void * that you define */
    int *blocktime;     /* Total time that a philosopher is blocked */
    int *blockstarting; /* If a philsopher is currently blocked, the time that he
                           started blocking */
    int phil_count;
    pthread_mutex_t *blockmon; /* monitor for blocktime */
} Phil_struct;

typedef struct sticks {
  pthread_mutex_t **lock;
  int phil_count;
} Sticks;


void pickup(Phil_struct *ps)
{
  Sticks *pp;
  int phil_count;

  pp = (Sticks *) ps->v;
  phil_count = pp->phil_count;
  
  if (ps->id % 2 == 1) {
    pthread_mutex_lock(pp->lock[ps->id]);       /* lock up left stick */
    pthread_mutex_lock(pp->lock[(ps->id+1)%phil_count]); /* lock right stick */ 
  } else {
    pthread_mutex_lock(pp->lock[(ps->id+1)%phil_count]); /* lock right stick */ 
    pthread_mutex_lock(pp->lock[ps->id]);       /* lock up left stick */
  }
}

void putdown(Phil_struct *ps)
{
  Sticks *pp;
  int i;
  int phil_count;

  pp = (Sticks *) ps->v;
  phil_count = pp->phil_count;

  if (ps->id % 2 == 1) {
    pthread_mutex_unlock(pp->lock[(ps->id+1)%phil_count]); /* unlock right stick */ 
    pthread_mutex_unlock(pp->lock[ps->id]);  /* unlock left stick */
  } else {
    pthread_mutex_unlock(pp->lock[ps->id]);  /* unlock left stick */
    pthread_mutex_unlock(pp->lock[(ps->id+1)%phil_count]); /* unlock right stick */ 
  }
}

void *initialize_v(int phil_count)
{
  Sticks *pp;
  int i;

  pp = (Sticks *) malloc(sizeof(Sticks));
  pp->phil_count = phil_count;
  pp->lock = (pthread_mutex_t **) malloc(sizeof(pthread_mutex_t *)*phil_count);
  if (pp->lock == NULL) { perror("malloc"); exit(1); }
  for (i = 0; i < phil_count; i++) {
    pp->lock[i] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if (pp->lock[i] == NULL) { perror("malloc"); exit(1); }
  }
  for (i = 0; i < phil_count; i++) {
    pthread_mutex_init(pp->lock[i], NULL);
  }

  return (void *) pp;
}
