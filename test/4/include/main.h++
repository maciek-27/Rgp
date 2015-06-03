#ifndef __MAIN_H__
#define __MAIN_H__
#include<pthread.h>
void err(const char *s);
extern pthread_mutex_t startSequenceMutex;

#endif
