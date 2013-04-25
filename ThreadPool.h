#ifndef ThreadPool_h
#define ThreadPool_h

// START:thread
#include <string>
#include <deque>
// START_HIGHLIGHT
#include <thread>
// END_HIGHLIGHT

#include "Work.h"

class ThreadPool {
public:
   virtual ~ThreadPool() {
      if (workThread_)
         workThread_->join();
   }

   void start() {
      workThread_ = std::make_shared<std::thread>(&ThreadPool::worker, this);
   }
   // ...
// END:thread
   bool hasWork() {
      return !workQueue_.empty();
   }

   void add(Work work) {
      workQueue_.push_front(work); 
   }

   Work pullWork() {
      auto work = workQueue_.back();
      workQueue_.pop_back();
      return work;
   }

// START:thread
private:
   void worker() {
      while (!hasWork())
         ;
      pullWork().execute();
   }

   std::deque<Work> workQueue_;
   std::shared_ptr<std::thread> workThread_;
};
// END:thread

#endif
