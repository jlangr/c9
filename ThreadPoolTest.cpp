#include "CppUTest/TestHarness.h"

#include "ThreadPool.h"
#include <functional>

using namespace std;

TEST_GROUP(AThreadPool) {};

TEST(AThreadPool, ProcessesASingleRequest) {
   ThreadPool pool;

   bool wasExecuted{false};
   auto executeFunction = [&] () { 
      wasExecuted = true; 
      // block
   };
   Work work(executeFunction);
   
   pool.add(work);

   // block until
}
