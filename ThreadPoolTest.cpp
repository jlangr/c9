#include "CppUTest/TestHarness.h"

#include "ThreadPool.h"

#include <condition_variable>
#include <mutex>
#include <chrono>

using namespace std;
using std::chrono::milliseconds;

TEST_GROUP(AThreadPool) {
   mutex m;
   ThreadPool pool;
};

TEST(AThreadPool, HasNoWorkOnCreation) {
   CHECK_FALSE(pool.hasWork());
}

TEST(AThreadPool, HasWorkAfterAdd) {
   pool.add(Work{});

   CHECK_TRUE(pool.hasWork());
}

TEST(AThreadPool, AnswersWorkAddedOnPull) {
   pool.add(Work{1});

   auto work = pool.pullWork();

   LONGS_EQUAL(1, work.id());
}

TEST(AThreadPool, PullsElementsInFIFOOrder) {
   pool.add(Work{1});
   pool.add(Work{2});

   auto work = pool.pullWork();
   LONGS_EQUAL(1, work.id());
}

TEST(AThreadPool, HasNoWorkAfterLastElementRemoved) {
   pool.add(Work{});

   pool.pullWork();

   CHECK_FALSE(pool.hasWork());
}

TEST(AThreadPool, HasWorkAfterWorkRemovedButWorkRemains) {
   pool.add(Work{});
   pool.add(Work{});

   pool.pullWork();

   CHECK_TRUE(pool.hasWork());
}

TEST(AThreadPool, PullsWorkInAThread) {
   condition_variable wasExecuted;
   Work work{[&] { wasExecuted.notify_all(); } };

   pool.add(work);

   unique_lock<mutex> lock(m);
   LONGS_EQUAL(cv_status::no_timeout, 
         wasExecuted.wait_for(lock, milliseconds(100)));
}
