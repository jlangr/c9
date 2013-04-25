#ifndef ThreadPool_h
#define ThreadPool_h

#include <string>
#include <thread>
#include <deque>

#include "Work.h"

class ThreadPool {
public:
// START:race
   ThreadPool() 
      : thread{std::make_shared<std::thread>(&ThreadPool::worker, this)} {}
// ...
// END:race
   ~ThreadPool() {
      thread->join();
   }

// START:race
   void worker() {
// START_HIGHLIGHT
      while (workQueue.empty())
// END_HIGHLIGHT
         ;

      Work work = workQueue.back();
      workQueue.pop_back();
      work.execute();
   }

   void add(Work work) {
      workQueue.push_front(work); 
   }
// END:race

private:
   std::shared_ptr<std::thread> thread;
   std::deque<Work> workQueue;
};

#endif
