#ifndef ThreadPool_h
#define ThreadPool_h

// START:threads
#include <string>
#include <deque>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
// START_HIGHLIGHT
#include <vector>
// END_HIGHLIGHT

#include "Work.h"

class ThreadPool {
public:
   // ...
// END:threads
   virtual ~ThreadPool() {
      stop();
   }

// START:threads
   void stop() {
      done_ = true;
// START_HIGHLIGHT
      for (auto& thread: threads_) thread.join();
// END_HIGHLIGHT
   }

// START_HIGHLIGHT
   void start(unsigned int numberOfThreads=1) {
      for (unsigned int i{0u}; i < numberOfThreads; i++)
         threads_.push_back(std::thread(&ThreadPool::worker, this));
   }
// END_HIGHLIGHT
   // ...
// END:threads

   bool hasWork() {
      std::lock_guard<std::mutex> block(mutex_);
      return !workQueue_.empty();
   }

   void add(Work work) {
      std::lock_guard<std::mutex> block(mutex_);
      workQueue_.push_front(work); 
   }

// START:threads_fix
   Work pullWork() {
      std::lock_guard<std::mutex> block(mutex_);
      
// START_HIGHLIGHT
      if (workQueue_.empty()) return Work{};
// END_HIGHLIGHT

      auto work = workQueue_.back();
      workQueue_.pop_back();
      return work;
   }
// END:threads_fix

// START:threads
private:
   // ...
// END:threads
   void worker() {
      while (!done_) {
         while (!done_ && !hasWork()) 
            ;
         if (done_) break;
         pullWork().execute();
      }
   }

// START:threads
   std::atomic<bool> done_{false};
   std::deque<Work> workQueue_;
   std::shared_ptr<std::thread> workThread_;
   std::mutex mutex_;
// START_HIGHLIGHT
   std::vector<std::thread> threads_;
// END_HIGHLIGHT
};
// END:threads

#endif
