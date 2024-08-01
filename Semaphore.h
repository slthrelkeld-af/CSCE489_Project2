#ifndef SEMAPHORE_H
#define SEMAPHORE_H

class Semaphore 
{
public:

	Semaphore(int count);
	~Semaphore();

	void wait();
	void signal();

private:

	int resourceCount; //initialize count of resources/queued processes
    pthread_cond_t conditionVar; //pthread condition variable
    pthread_mutex_t mutex; //pthread mutex for locking the semaphore condition variable

};

#endif
