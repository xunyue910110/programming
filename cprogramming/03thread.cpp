#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <chrono>
#include <condition_variable>
using namespace std;


std::deque<int> q;
std::mutex mu;
std::condition_variable cond;

void function_1 () {
  int unfilled = 10;
  while ( unfilled > 0) {
    std::unique_lock<mutex> locker(mu);
   q.push_front(unfilled) ;
   locker.unlock();
   cond.notify_one();
   std::this_thread::sleep_for(chrono::seconds(1));
   unfilled--;
  }
}

void function_2(){
  int data = 0;
  while ( data != 1){
    std::unique_lock<mutex> locker(mu);
    cond.wait(locker, [](){return !q.empty();});
       data = q.back();
      q.pop_back();
     locker.unlock();
    cout << "t2 got a vlue from t1: "  << data << endl;
  }
}

int main() {
  std::thread t1(function_1);
  std::thread t2(function_2);
  t1.join();
  t2.join();
}
