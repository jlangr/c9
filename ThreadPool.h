#ifndef ThreadPool_h
#define ThreadPool_h

// START:thread
#include <string>
#include <deque>
#include <thread>
#include <memory>
// START_HIGHLIGHT
#include <atomic>
// END_HIGHLIGHT

#include "Work.h"

class ThreadPool {
public:
   virtual ~ThreadPool() {
// START_HIGHLIGHT
      done_ = true;
// END_HIGHLIGHT
      if (workThread_)
         workThread_->join();
   }
   // ...
// END:thread
   void start() {
      workThread_ = std::make_shared<std::thread>(&ThreadPool::worker, this);
   }

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
// START_HIGHLIGHT
      while (!done_) {
// END_HIGHLIGHT
         while (!hasWork())
            ;
         pullWork().execute();
// START_HIGHLIGHT
      }
// END_HIGHLIGHT
   }

// START_HIGHLIGHT
   std::atomic<bool> done_{false};
// END_HIGHLIGHT
   std::deque<Work> workQueue_;
   std::shared_ptr<std::thread> workThread_;
};
// END:thread

#endif
