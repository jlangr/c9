#include "CppUTest/TestHarness.h"
#include "CppUTestExtensions.h"

#include <condition_variable>
#include <thread>
#include <chrono>

#include "GeoServer.h"
#include "VectorUtil.h"
#include "TestTimer.h"
#include "ThreadPool.h"
#include "Work.h"

using namespace std;
using std::chrono::milliseconds;

// START:assertfirsthelper
TEST_GROUP(AGeoServer) {
   // ...
// END:assertfirsthelper
   GeoServer server;

   const string aUser{"auser"};
   const double LocationTolerance{0.005};

// START:assertfirsthelper
// START_HIGHLIGHT
   bool locationIsUnknown(const string& user) {
      return server.locationOf(user).isUnknown();
   }
// END_HIGHLIGHT
};
// END:assertfirsthelper

// START:assertfirst
TEST(AGeoServer, AnswersUnknownLocationWhenUserNoLongerTracked) {
   server.track(aUser);

   server.stopTracking(aUser);

   CHECK_TRUE(locationIsUnknown(aUser));
}
// END:assertfirst

TEST(AGeoServer, TracksAUser) {
   server.track(aUser);

   CHECK_TRUE(server.isTracking(aUser));
}

TEST(AGeoServer, IsNotTrackingAUserNotTracked) {
   CHECK_FALSE(server.isTracking(aUser));
}

TEST(AGeoServer, TracksMultipleUsers) {
   server.track(aUser);
   server.track("anotheruser");

   CHECK_FALSE(server.isTracking("thirduser"));
   CHECK_TRUE(server.isTracking(aUser));
   CHECK_TRUE(server.isTracking("anotheruser"));
}

TEST(AGeoServer, IsTrackingAnswersFalseWhenUserNoLongerTracked) {
   server.track(aUser);
   
   server.stopTracking(aUser);

   CHECK_FALSE(server.isTracking(aUser));
}

TEST(AGeoServer, UpdatesLocationOfUser) {
   server.track(aUser);

   server.updateLocation(aUser, Location{38, -104});

   auto location = server.locationOf(aUser);
   DOUBLES_EQUAL(38, location.latitude(), LocationTolerance);
   DOUBLES_EQUAL(-104, location.longitude(), LocationTolerance);
}

TEST(AGeoServer, AnswersUnknownLocationForUserNotTracked) {
   CHECK_TRUE(server.locationOf("anAbUser").isUnknown());
}

TEST(AGeoServer, AnswersUnknownLocationForTrackedUserWithNoLocationUpdate) {
   server.track(aUser);

   CHECK_TRUE(server.locationOf(aUser).isUnknown());
}

TEST(AGeoServer, AnswersUnknownLocationForUserNoLongerTracked) {
   server.track(aUser);
   server.updateLocation(aUser, Location(40, 100));

   server.stopTracking(aUser);

   CHECK_TRUE(server.locationOf(aUser).isUnknown());
}

class GeoServerUsersInBoxTests: public Utest {
public:
   GeoServer server;

   const double TenMeters { 10 };
   const double Width { 2000 + TenMeters };
   const double Height { 4000 + TenMeters};
   const string aUser { "auser" };
   const string bUser { "buser" };
   const string cUser { "cuser" };

   Location aUserLocation { 38, -103 };

   shared_ptr<ThreadPool> pool;

   virtual void setup() override {
      server.useThreadPool(pool);

      server.track(aUser);
      server.track(bUser);
      server.track(cUser);

      server.updateLocation(aUser, aUserLocation);
   }

   void addUsersAt(unsigned int number, const Location& location) {
      for (unsigned int i{0}; i < number; i++) {
         string user{"user" + to_string(i)};
         server.track(user);
         server.updateLocation(user, location);
      }
   }
};

TEST_GROUP_BASE(AGeoServer_UsersInBox, GeoServerUsersInBoxTests) {
   class GeoServerUserTrackingListener: public GeoServerListener {
   public:
      void updated(const User& user) { Users.push_back(user); }
      vector<User> Users;
   } trackingListener;

   class SingleThreadedPool: public ThreadPool {
   public:
      virtual void add(Work work) override { work.execute(); }
   };

   void setup() override {
      pool = make_shared<SingleThreadedPool>();
      GeoServerUsersInBoxTests::setup();
   }

   vector<string> UserNames(const vector<User>& users) {
      return Collect<User,string>(users, [](User each) { return each.name(); });
   }
};

TEST(AGeoServer_UsersInBox, AnswersUsersInSpecifiedRange) {
   pool->start(0);
   server.updateLocation(
      bUser, Location{aUserLocation.go(Width / 2 - TenMeters, East)}); 

   server.usersInBox(aUser, Width, Height, &trackingListener);

   CHECK_EQUAL(vector<string> { bUser }, UserNames(trackingListener.Users));
}

TEST(AGeoServer_UsersInBox, AnswersOnlyUsersWithinSpecifiedRange) {
   pool->start(0);
   server.updateLocation(
      bUser, Location{aUserLocation.go(Width / 2 + TenMeters, East)}); 
   server.updateLocation(
      cUser, Location{aUserLocation.go(Width / 2 - TenMeters, East)}); 

   server.usersInBox(aUser, Width, Height, &trackingListener);

   CHECK_EQUAL(vector<string> { cUser }, UserNames(trackingListener.Users));
}

TEST(AGeoServer_UsersInBox, HandlesLargeNumbersOfUsers) {
   pool->start(0);
   const unsigned int lots {5000};
   addUsersAt(lots, Location{aUserLocation.go(TenMeters, West)});

   TestTimer timer;
   server.usersInBox(aUser, Width, Height, &trackingListener);

   LONGS_EQUAL(lots, trackingListener.Users.size());
}

// START:pool
TEST_GROUP_BASE(AGeoServer_ScaleTests, GeoServerUsersInBoxTests) {
   class GeoServerCountingListener: public GeoServerListener {
   public:
      void updated(const User& user) override {
         unique_lock<std::mutex> lock(mutex_);
         Count++;
         wasExecuted_.notify_all();
      }

      void waitForCountAndFailOnTimeout(unsigned int expectedCount, 
            const milliseconds& time=milliseconds(10000)) {
         unique_lock<mutex> lock(mutex_);
         CHECK_TRUE(wasExecuted_.wait_for(lock, time, [&] 
                  { return expectedCount == Count; }));
      }

      condition_variable wasExecuted_;
      unsigned int Count{0};
      mutex mutex_;
   };

   GeoServerCountingListener countingListener;
   shared_ptr<thread> t;

   void setup() override {
      pool = make_shared<ThreadPool>();
      GeoServerUsersInBoxTests::setup();
   }

   void teardown() override {
      t->join();
   }
};

TEST(AGeoServer_ScaleTests, HandlesLargeNumbersOfUsers) {
   pool->start(4);
   const unsigned int lots{5000};
   addUsersAt(lots, Location{aUserLocation.go(TenMeters, West)});

   t = make_shared<thread>(
         [&] { server.usersInBox(aUser, Width, Height, &countingListener); });

   countingListener.waitForCountAndFailOnTimeout(lots);
}
// END:pool

