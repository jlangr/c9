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

TEST_GROUP(AGeoServer) {
   GeoServer server;

   const string aUser{"auser"};
   const double LocationTolerance{0.005};
};

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

TEST_GROUP(AGeoServer_UsersInBox) {
   GeoServer server;

   const double TenMeters { 10 };
   const double Width { 2000 + TenMeters };
   const double Height { 4000 + TenMeters};
   const string aUser { "auser" };
   const string bUser { "buser" };
   const string cUser { "cuser" };

   Location aUserLocation { 38, -103 };

   class GeoServerUserTrackingListener: public GeoServerListener {
   public:
      void updated(const User& user) { Users.push_back(user); }
      vector<User> Users;
   } trackingListener;

   class SingleThreadedPool: public ThreadPool {
   public:
      virtual void add(Work work) override { work.execute(); }
   };
   shared_ptr<ThreadPool> pool;

   void setup() {
      pool = make_shared<SingleThreadedPool>();
      server.useThreadPool(pool);
      server.track(aUser);
      server.track(bUser);
      server.track(cUser);

      server.updateLocation(aUser, aUserLocation);
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

IGNORE_TEST(AGeoServer_UsersInBox, HandlesLargeNumbersOfUsers) {
   pool->start(0);
   Location anotherLocation{aUserLocation.go(10, West)};
   const unsigned int lots {500000};
   for (unsigned int i{0}; i < lots; i++) {
      string user{"user" + to_string(i)};
      server.track(user);
      server.updateLocation(user, anotherLocation);
   }

   TestTimer timer;
   server.usersInBox(aUser, Width, Height, &trackingListener);

   CHECK_EQUAL(lots, trackingListener.Users.size());
}

mutex mm;

TEST_GROUP(AGeoServer_ScaleTests) {
   GeoServer server;

   const double TenMeters { 10 };
   const double Width { 2000 + TenMeters };
   const double Height { 4000 + TenMeters};
   const string aUser { "auser" };
   const string bUser { "buser" };
   const string cUser { "cuser" };

   Location aUserLocation { 38, -103 };
   condition_variable wasExecuted;

   class GeoServerCountingListener: public GeoServerListener {
   public:
      GeoServerCountingListener(condition_variable* condition) 
         : condition_{condition} {}

      void updated(const User& user) override {
         unique_lock<std::mutex> lock(mm);
         Count++;
         condition_->notify_all();
      }

      void waitForCountAndFailOnTimeout(unsigned int expectedCount, 
            const milliseconds& time=milliseconds(10000)) {
         unique_lock<mutex> lock(mm);
         CHECK_TRUE(condition_->wait_for(lock, time, 
             [&] { return expectedCount == Count; }));
      }

      condition_variable* condition_;
      unsigned int Count{0};
   };

   GeoServerCountingListener countingListener{&wasExecuted};
   shared_ptr<ThreadPool> pool { make_shared<ThreadPool>() };
   shared_ptr<thread> t;

   void setup() {
      server.track(aUser);
      server.track(bUser);
      server.track(cUser);

      server.updateLocation(aUser, aUserLocation);
      
      server.useThreadPool(pool);
   }

   void teardown() {
      t->join();
   }

   void addUsersAt(unsigned int number, const Location& location) {
      for (unsigned int i{0}; i < number; i++) {
         string user{"user" + to_string(i)};
         server.track(user);
         server.updateLocation(user, location);
      }
   }
};

TEST(AGeoServer_ScaleTests, HandlesLargeNumbersOfUsers) {
   pool->start(4);
   const unsigned int lots{5000};
   Location anotherLocation{aUserLocation.go(TenMeters, West)};
   for (unsigned int i{0}; i < lots; i++) {
      string user{"user" + to_string(i)};
      server.track(user);
      server.updateLocation(user, anotherLocation);
   }

   t = make_shared<thread>(
         [&] { server.usersInBox(aUser, Width, Height, &countingListener); });

   countingListener.waitForCountAndFailOnTimeout(lots);
}


