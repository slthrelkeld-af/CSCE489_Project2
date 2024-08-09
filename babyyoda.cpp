/*************************************************************************************
 * babyyoda - used to test your semaphore implementation and can be a starting point for
 *			     your store front implementation
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "Semaphore.h"

// Semaphores that each thread will have access to as they are global in shared memory
Semaphore *empty = NULL;
Semaphore *full = NULL;

pthread_mutex_t buf_mutex;

int* buffer = nullptr; //pointer to future buffer array
//ref'd stack exchange: https://stackoverflow.com/questions/63888588/create-c-variable-length-global-arrays
int consumed = 0;
int buffer_size;
int consumer_pointer;
int num_produce;


/*************************************************************************************
 * producer_routine - this function is called when the producer thread is created.
 *
 *			Params: data - a void pointer that should point to an integer that indicates
 *							   the total number to be produced
 *
 *			Returns: always NULL
 *
 *************************************************************************************/

void *producer_routine(void *data) {

	time_t rand_seed;
	srand((unsigned int) time(&rand_seed));

	// The current serial number (incremented)
	int serialnum = 1;
	
	// We know the data pointer is an integer that indicates the number to produce
	int left_to_produce = *((int *) data);

	int producer_buffer_pointer = 0;

	// Loop through the amount we're going to produce and place into the buffer
	while (left_to_produce > 0) {
		printf("Producer wants to put Yoda #%d into buffer...\n", serialnum);

		// Semaphore check to make sure there is an available slot
		full->wait();

		// Place item on the next shelf slot by first setting the mutex to protect our buffer vars
		pthread_mutex_lock(&buf_mutex);

		buffer[producer_buffer_pointer] = serialnum;
		serialnum++;
		left_to_produce--;
		
		pthread_mutex_unlock(&buf_mutex);
		
//		printf("   Yoda %d put in buffer %d.\n", buffer[producer_buffer_pointer], producer_buffer_pointer); //test code to assure varible length buffer use
		producer_buffer_pointer = ((producer_buffer_pointer + 1)%buffer_size);
		
		// Semaphore signal that there are items available
		empty->signal();

		// random sleep but he makes them fast so 1/20 of a second
		usleep((useconds_t) (rand() % 200000));
	
	}
	return NULL;
}


/*************************************************************************************
 * consumer_routine - this function is called when the consumer thread is created.
 *
 *       Params: data - a void pointer that should point to a boolean that indicates
 *                      the thread should exit. Doesn't work so don't worry about it
 *
 *       Returns: always NULL
 *
 *************************************************************************************/

void *consumer_routine(void *data) {
	(void) data;

	bool quitthreads = false;

	while (!quitthreads) {
		printf("Consumer wants to buy a Yoda...\n");

		// Semaphore to see if there are any items to take
		empty->wait();

		if (consumed >= num_produce) // Should not require mutex protection (since EDITS to consumed are mutex locked)
		{
			empty->signal(); //increasing the semaphore to release consumers from indefinite waits. Would break future functionality, but we no longer need it.
			break;
		}
		
		// Take an item off the shelf
		pthread_mutex_lock(&buf_mutex);
	
		printf("   Consumer bought Yoda #%d.\n", buffer[consumer_pointer]);
		buffer[consumer_pointer] = 0;
		consumed++;
		//max efficiency might set up a separate mutex to protect the customer pointer, but it should be quick enough that I'm going to leave it combined
		consumer_pointer = (consumer_pointer + 1) % buffer_size;

		pthread_mutex_unlock(&buf_mutex);

		// Consumers wait up to one second
		usleep((useconds_t) (rand() % 1000000));

		full->signal();

		if (consumed >= num_produce)
		{
			empty->signal(); //increasing the semaphore to release consumers from indefinite waits. Would break future functionality, but we no longer need it.
			break;
		}
		
	}
	printf("Consumer goes home\n");
	return NULL;	
}


/*************************************************************************************
 * main - Standard C main function for our storefront. 
 *
 *		Original params: pctest <num_consumers> <max_items> //this is not actually accurate, is bull. First argument actually sets num_produce and no additonal arguments are referenced.
 *		Project desired params: babyyoda <buffer_size>  <num_consumers> < max_items>
 *				max_items - how many items will be produced before the shopkeeper closes
 *
 *************************************************************************************/

int main(int argv, const char *argc[]) {

	// Validate argument parameters. Will not error out when encountering additional parameters, but will ignore any additional arguments.
	if (argv < 4) {
		printf("Invalid parameters. Format: %s <buffer_size> <num_consumers> <max_items>\n", argc[0]);
		exit(0);
	}

	for (int i = 1; i < 4; i++){
		if (strtol(argc[i], NULL, 10) < 1)
		{
			printf("Invalid parameter. Parameter %d is not a positive nonzero integer\n", i);
			exit(0);
		}
	}

	// User input on the number of integers in the buffer
	buffer_size = (int) strtol(argc[1], NULL, 10);
	buffer = new int[buffer_size];
	for (int i=0; i < buffer_size; i++){ //initialize full buffer to zero
		buffer[i]=0;	
	}

	// User input on number of consumer threads
	unsigned long num_consumers = (unsigned long) strtol(argc[2], NULL, 10);

	// User input on the number of producer repeats
	num_produce = (int) strtol(argc[3], NULL, 10);


	printf("Producing %d today.\n", num_produce);
	
	// Initialize our semaphores
	empty = new Semaphore(0);
	full = new Semaphore((int)buffer_size);

	pthread_mutex_init(&buf_mutex, NULL); // Initialize our buffer mutex

	pthread_t producer;
	pthread_t consumers[num_consumers] = {0};

	// Launch our producer thread
	pthread_create(&producer, NULL, producer_routine, (void *) &num_produce);

	// Launch our consumer threads
	for (pthread_t consumer: consumers)
	{
		pthread_create(&consumer, NULL, consumer_routine, NULL);
	}
	
	// Wait for our producer thread to finish up
	pthread_join(producer, NULL);

	printf("The manufacturer has completed his work for the day.\n");

	// Give the consumers a second to finish snatching up items
	while (consumed < num_produce){
		printf("Waiting for consumer to buy up the rest.\n");
		sleep(1);
	}
	// Now make sure they all exited
	for (pthread_t consumer: consumers) {
		pthread_join(consumer, NULL);
	}

	// We are exiting, clean up
	delete empty;
	delete full;		

	printf("Producer/Consumer simulation complete!\n");

}
