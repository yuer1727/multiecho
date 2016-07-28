
#ifndef _mythread_h_
#define _mythread_h_

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <string>


using namespace std;

class Mythread{

  public : 
     pthread_t tid;
  private :
     static pthread_mutex_t mutex;
     
  public :
     Mythread();
     int Create(void* Callback, void* args);
     int Join();

     static int InitMutex();
     static int LockMutex(const char *identifier);
     static int UnLockMutex(const char *identifier);

};

#endif
