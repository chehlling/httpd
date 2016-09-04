#include "locker.h"

locker::locker()
{
	pthread_mutex_init(&mutex,NULL);
}
locker::~locker()
{
	pthread_mutex_destroy(&mutex);
}
void locker::lock()
{	
	pthread_mutex_lock(&mutex);
}
void locker::unlock()
{	
	pthread_mutex_unlock(&mutex);
}
sem::sem()
{
	sem_init(&msem,0,0);
}
sem::~sem()
{
	sem_destroy(&msem);
}
bool sem::wait()
{
	return sem_wait(&msem)==0;
}	
bool sem::post()
{
	return sem_post(&msem)==0;
}
