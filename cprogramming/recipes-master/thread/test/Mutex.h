#include <pthread.h>

class MutexLock {
  private:
    pthread_mutex_t mutex;
  public:
    MutexLock()
    {
      pthread_mutex_init(&mutex,NULL);
    }
    ~MutexLock(){
      pthread_mutex_destroy(&mutex);
    }
    void lock(){
      pthread_mutex_lock(&mutex);
    }
    void unlock(){
      pthread_mutex_unlock(&mutex);
    }
};
class  MutexLockGuard{
  public:
  explicit MutexLockGuard(MutexLock& mutex):
    _mutex(mutex){
      _mutex.lock();
    }
  ~MutexLockGuard(){
    _mutex.unlock();
  }
  private:
    MutexLock& _mutex;
};


