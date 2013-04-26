#ifndef ThreadPool_h
#define ThreadPool_h

// START:thread
#include <string>
#include <deque>
#include <thread>
#include <memory>
#include <atomic>
// START_HIGHLIGHT
#include <mutex>
// END_HIGHLIGHT

#include "Work.h"

class ThreadPool {
public:
   // ...
// END:thread
   virtual ~ThreadPool() {
      stop();
   }

   void stop() {
      done_ = true;
      if (workThread_)
         workThread_->join();
   }

   void start() {
      workThread_ = std::make_shared<std::thread>(&ThreadPool::worker, this);
   }

// START:thread
   bool hasWork() {
// START_HIGHLIGHT
      std::lock_guard<std::mutex> block(mutex_);
// END_HIGHLIGHT
      return !workQueue_.empty();
   }

   void add(Work work) {
// START_HIGHLIGHT
      std::lock_guard<std::mutex> block(mutex_);
// END_HIGHLIGHT
      workQueue_.push_front(work); 
   }

   Work pullWork() {
// START_HIGHLIGHT
      std::lock_guard<std::mutex> block(mutex_);
// END_HIGHLIGHT

      auto work = workQueue_.back();
      workQueue_.pop_back();
      return work;
   }
   // ...
// END:thread

private:
   void worker() {
      while (!done_) {
         while (!done_ && !hasWork()) 
            ;
         if (done_) break;
         pullWork().execute();
      }
   }

// START:thread
   std::atomic<bool> done_{false};
   std::deque<Work> workQueue_;
   std::shared_ptr<std::thread> workThread_;
// START_HIGHLIGHT
   std::mutex mutex_;
// END_HIGHLIGHT
};
// END:thread

#endif
