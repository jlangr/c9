#include "CppUTest/TestHarness.h"

#include "ThreadPool.h"
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>

using namespace std;

TEST_GROUP(AThreadPool) {
   mutex m;
};

TEST(AThreadPool, ProcessesASingleRequest) {
   ThreadPool pool;
   condition_variable wasExecuted;
   auto executeFunction = [&] () { 
      wasExecuted.notify_one();
   };
   Work work(executeFunction);
   
   pool.add(work);

   unique_lock<mutex> lock(m);
   chrono::milliseconds timeout(100);
   CHECK_FALSE(wasExecuted.wait_for(lock, timeout) 
         == cv_status::timeout);
}

