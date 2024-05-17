#include <pthread.h>

struct station {
	// FILL ME IN
	int numberOfEmptySeats ;
	int numberOfWaitingPassengers ;
	int numberOfPassengersWalkingOnTheTrain;
	pthread_mutex_t mutex ;
	
       pthread_cond_t emptyCond ;
       pthread_cond_t fullCond ;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);
