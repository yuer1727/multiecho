
#include "mythread.h"

using namespace std;

pthread_mutex_t Mythread::mutex;

Mythread::Mythread(){

}

int Mythread::Create(void* Callback, void* args){

     int ret = 0;
     ret  = pthread_create(&this->tid, NULL,  (void*(*)(void*))Callback, args);

     if(ret){
       cerr << "Error while creating threads. " << endl;
       return ret;
     }
     else {
          cout << "Thread successfully created. " << endl;
          return 0;
     }
}

int Mythread::Join(){
    pthread_join(this->tid, NULL);
    return 0;
}

int Mythread::InitMutex(){
   if(pthread_mutex_init(&Mythread::mutex, NULL) < 0){
      cerr << "Error while initializing mutex" << endl;
      return -1;
   }
   else{
      cout << "Mutex initialized." << endl;
      return 0;
   }
}

int Mythread::LockMutex(const char *identifier) {
 // cout << identifier << " is trying to acquire the lock..." << endl;
  if(pthread_mutex_lock(&Mythread::mutex) == 0) {
   // cout << identifier << " acquired the lock!" << endl;
    return 0;
  }
  else {
   cerr << "Error while " << identifier << " was trying to acquire the lock" << endl;
   return -1;
  }
}

int Mythread::UnLockMutex(const char *identifier) {
  // cout << identifier << " is trying to release the lock..." << endl;
  if(pthread_mutex_unlock(&Mythread::mutex) == 0) {
    // cout << identifier << " released the lock!" << endl;
    return 0;
  }
  else {
   cerr << "Error while " << identifier << " was trying to release the lock" << endl;
   return -1;
  }
}








