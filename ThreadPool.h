#ifndef ThreadPool_h
#define ThreadPool_h

#include <string>
#include <thread>
#include <deque>

#include "Work.h"

class ThreadPool {
public:
   ThreadPool() 
      : thread{std::make_shared<std::thread>(&ThreadPool::worker, this)} {}

   ~ThreadPool() {
      thread->join();
   }

   void worker() {
      while (workQueue.empty())
         ;

      Work work = workQueue.back();
      workQueue.pop_back();
      work.execute();
   }

   void add(Work work) {
      workQueue.push_front(work); 
   }

private:
   std::shared_ptr<std::thread> thread;
   std::deque<Work> workQueue;
};

#endif
