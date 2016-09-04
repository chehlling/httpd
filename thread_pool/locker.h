#pragma once
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

class locker
{
public:
	locker();
	~locker();
	void lock();
	void unlock();
private:
	pthread_mutex_t mutex;
};

class sem
{
public:
	sem();
	~sem();
	bool wait();
	bool post();
private:	
	sem_t msem;
};

