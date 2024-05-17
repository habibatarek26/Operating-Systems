#include "caltrain.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void station_init(struct station *station) {
	  station->numberOfEmptySeats = 0;
	  station->numberOfWaitingPassengers = 0;
	  station->numberOfPassengersWalkingOnTheTrain = 0;

	  pthread_mutex_init(&station->mutex, NULL);
	  pthread_cond_init(&station->emptyCond, NULL);
	  pthread_cond_init(&station->fullCond, NULL);
}

void station_load_train(struct station *station, int count) {
  pthread_mutex_lock(&station->mutex);

      if (station->numberOfWaitingPassengers > 0) {
	    // intialize number of train seats
	    station->numberOfEmptySeats = count;

	    // Signal to waiting passengers that the train has arrived and there are
	    // empty seats
	    pthread_cond_broadcast(&station->emptyCond);

	    // Wait until either the train is full or there are no more waiting
	    // passengers
	    while (station->numberOfEmptySeats > 0 &&
		   station->numberOfWaitingPassengers > 0) {
	      pthread_cond_wait(&station->fullCond, &station->mutex);
	    }
      }
	    pthread_mutex_unlock(&station->mutex);
}

void station_wait_for_train(struct station *station) {
	  pthread_mutex_lock(&station->mutex);

	  // Increment the number of waiting passengers
	  station->numberOfWaitingPassengers++;

	  // Wait until there are empty seats on the train
	  while (station->numberOfEmptySeats == 0)
	    pthread_cond_wait(&station->emptyCond, &station->mutex);

	  // Decrement the number of waiting passengers as they board the train;
	  station->numberOfWaitingPassengers--;
	  station->numberOfEmptySeats--;
	  station->numberOfPassengersWalkingOnTheTrain++;

	  pthread_mutex_unlock(&station->mutex);
}

void station_on_board(struct station *station) {
	  pthread_mutex_lock(&station->mutex);
	  // when  a passenger sets
	  if (station->numberOfPassengersWalkingOnTheTrain > 0)
	     station->numberOfPassengersWalkingOnTheTrain--;

	  // Signal that the train is full if there are no more empty seats or no more
	  // waiting passengers
	  if ((station->numberOfEmptySeats == 0 ||
	     station->numberOfWaitingPassengers == 0) &&
	     station->numberOfPassengersWalkingOnTheTrain == 0)
	       pthread_cond_signal(&station->fullCond);

	  pthread_mutex_unlock(&station->mutex);
}
