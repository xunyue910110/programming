#include <iostream>
#include <future>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <deque>
using namespace std;

std::deque<std::packaged_task<int()>> task_q;
mutex mu;
condition_variable cond;

int factorial(int n){
  int res = 1;
  for ( int i = n; i > 1; i--)
    res *= i;

  cout << "Result is : " << res << endl;
  return res;
}

void thread_1() {
 std::packaged_task<int()> t;
 {
   std::unique_lock<std::mutex> locker(mu);
   cond.wait(locker, [](){return !task_q.empty();});
   t = std::move(task_q.front());
   task_q.pop_front();
 }
 t();
}

int main() {

  thread t1 (thread_1);
  packaged_task<int()> t (std::bind(factorial, 6));
  future<int> fu = t.get_future();

  {
    std::unique_lock<std::mutex> locker(mu);
   task_q.push_back(std::move(t));
  }
  cond.notify_one();

  cout << fu.get() << endl;
  t1.join();
}
