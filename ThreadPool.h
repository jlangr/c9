#ifndef ThreadPool_h
#define ThreadPool_h

#include <string>
#include <deque>

#include "Work.h"

class ThreadPool {
public:
   bool hasWork() {
      return !workQueue.empty();
   }

   void add(Work work) {
      has = true;
      workQueue.push_front(work); 
   }

   Work pullWork() {
      Work work = workQueue.back();
      workQueue.pop_back();
      return work;
   }

private:
   std::deque<Work> workQueue;
};

#endif
