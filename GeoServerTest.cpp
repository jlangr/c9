#include "CppUTest/TestHarness.h"
#include "CppUTestExtensions.h"

#include "GeoServer.h"
#include "VectorUtil.h"
#include "TestTimer.h"
#include "ThreadPool.h"
#include "Work.h"

using namespace std;

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

// START:pool
TEST_GROUP(AGeoServer_UsersInBox) {
   GeoServer server;
   // ...
// END:pool

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

// START:pool
   class SingleThreadedPool: public ThreadPool {
   public:
      virtual void add(Work work) override { work.execute(); }
   };
   shared_ptr<ThreadPool> pool;

   void setup() {
      pool = make_shared<SingleThreadedPool>();
      server.useThreadPool(pool);
      // ...
// END:pool
      server.track(aUser);
      server.track(bUser);
      server.track(cUser);

      server.updateLocation(aUser, aUserLocation);
// START:pool
   }
   // ...
// END:pool

   vector<string> UserNames(const vector<User>& users) {
      return Collect<User,string>(users, [](User each) { return each.name(); });
   }
// START:pool
};

TEST(AGeoServer_UsersInBox, AnswersUsersInSpecifiedRange) {
   pool->start(0);
   server.updateLocation(
      bUser, Location{aUserLocation.go(Width / 2 - TenMeters, East)}); 

   server.usersInBox(aUser, Width, Height, &trackingListener);

   CHECK_EQUAL(vector<string> { bUser }, UserNames(trackingListener.Users));
}
// END:pool

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

