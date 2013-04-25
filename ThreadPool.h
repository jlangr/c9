#ifndef ThreadPool_h
#define ThreadPool_h

#include <string>
#include <deque>
#include <thread>

#include "Work.h"

class ThreadPool {
public:
   ThreadPool() 
      : workThread_{&ThreadPool::worker, this} {
   }

   virtual ~ThreadPool() {
      workThread_.join();
   }

   bool hasWork() {
      return !workQueue_.empty();
   }

   void add(Work work) {
      workQueue_.push_front(work); 
   }

   Work pullWork() {
      Work work = workQueue_.back();
      workQueue_.pop_back();
      return work;
   }

private:
   void worker() {
   }

   std::deque<Work> workQueue_;
   std::thread workThread_;
};

#endif
