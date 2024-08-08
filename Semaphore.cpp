#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "Semaphore.h"

/*************************************************************************************
 * Semaphore (constructor) - this should take count and place it into a local variable.
 *						Here you can do any other initialization you may need.
 *
 *    Params:  count - initialization count for the semaphore
 *
 *************************************************************************************/

//using the textbook as reference
//also using W3schools.com & geeksforgeeks because I don't know any C++ and we got no instruction on it
//pubs.opengroup.org/onlinepubs, 
//https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-pthread-cond-wait-wait-condition-variable, 
//https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-pthread-cond-signal-signal-condition#ptcsig for pthreads reference material
//why ask us what language we want to use and then not use it?

Semaphore::Semaphore(int count) {
    resourceCount = count; //initialize count of resources/queued processes

    if (pthread_mutex_init(&mutex, NULL) != 0) {                                  
        perror("pthread_mutex_init() error");                                       
        exit(1);
    }
    if (pthread_cond_init(&conditionVar, NULL) != 0) {                                    
        perror("pthread_cond_init() error");                                        
        exit(2);
    }

    
}


/*************************************************************************************
 * ~Semaphore (destructor) - called when the class is destroyed. Clean up any dynamic
 *						memory.
 *
 *************************************************************************************/

Semaphore::~Semaphore() {
    //
    //would be nice if we were actually taught us anything related to how to deallocate memory in c++ before we're expected to do it.
    pthread_cond_destroy(&conditionVar);
    pthread_mutex_destroy(&mutex);
}


/*************************************************************************************
 * wait - implement a standard wait Semaphore method here
 *
 *************************************************************************************/

void Semaphore::wait() {
    pthread_mutex_lock(&mutex);

    resourceCount --; //decrement resource count

    if (resourceCount < 0){
        pthread_cond_wait(&conditionVar, &mutex);
    }

    pthread_mutex_unlock(&mutex);
    
}


/*************************************************************************************
 * signal - implement a standard signal Semaphore method here
 *
 *************************************************************************************/

void Semaphore::signal() {
    pthread_mutex_lock(&mutex);

    resourceCount ++;
    
    if (resourceCount <= 0){
        pthread_cond_signal(&conditionVar);
    }

    pthread_mutex_unlock(&mutex);

}


