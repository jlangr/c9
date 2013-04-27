#ifndef ThreadPool_h
#define ThreadPool_h

#include <string>
#include <deque>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

#include "Work.h"

class ThreadPool {
public:
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

   bool hasWork() {
      std::lock_guard<std::mutex> block(mutex_);
      return !workQueue_.empty();
   }

   void add(Work work) {
      std::lock_guard<std::mutex> block(mutex_);
      workQueue_.push_front(work); 
   }

   Work pullWork() {
      std::lock_guard<std::mutex> block(mutex_);

//      if (workQueue_.empty()) return Work{};

      auto work = workQueue_.back();
      workQueue_.pop_back();
      return work;
   }

private:
   void worker() {
      while (!done_) {
         while (!done_ && !hasWork()) 
            ;
         if (done_) break;
         pullWork().execute();
      }
   }

   std::atomic<bool> done_{false};
   std::deque<Work> workQueue_;
   std::shared_ptr<std::thread> workThread_;
   std::mutex mutex_;
};

#endif
