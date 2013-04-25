#ifndef ThreadPool_h
#define ThreadPool_h

// START:thread
#include <string>
#include <deque>
// START_HIGHLIGHT
#include <thread>
#include <memory>
// END_HIGHLIGHT

#include "Work.h"

class ThreadPool {
public:
// START_HIGHLIGHT
   virtual ~ThreadPool() {
      if (workThread_)
         workThread_->join();
   }
// END_HIGHLIGHT

// START_HIGHLIGHT
   void start() {
      workThread_ = std::make_shared<std::thread>(&ThreadPool::worker, this);
   }
// END_HIGHLIGHT
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
// START_HIGHLIGHT
   void worker() {
      while (!hasWork())
         ;
      pullWork().execute();
   }
// END_HIGHLIGHT

   std::deque<Work> workQueue_;
// START_HIGHLIGHT
   std::shared_ptr<std::thread> workThread_;
// END_HIGHLIGHT
};
// END:thread

#endif
