#include "CppUTest/TestHarness.h"

#include <sstream>

#include "Location.h"

using namespace std;

SimpleString StringFrom(const Location& location) {
   return SimpleString(
         StringFromFormat("(%d, %d)", 
                          location.latitude(), location.longitude()));
}

TEST_GROUP(ALocation) {
   const double Tolerance { 0.005 };
   const Location ArbitraryLocation { 38.2, -104.5 };
};

TEST(ALocation, AnswersLatitudeAndLongitude) {
   Location location{10, 20};

   LONGS_EQUAL(10, location.latitude());
   LONGS_EQUAL(20, location.longitude());
}

TEST(ALocation, IsNotUnknownWhenLatitudeAndLongitudeProvided) {
   Location location{1, 1};

   CHECK_FALSE(location.isUnknown());
}

TEST(ALocation, IsUnknownWhenLatitudeAndLongitudeNotProvided) {
   Location location;

   CHECK_TRUE(location.isUnknown());
}

TEST(ALocation, AnswersDistanceFromAnotherInMeters) {
   Location point1{ 38.017, -104.84 };
   Location point2{ 38.025, -104.99 };

   // verified at www.ig.utexas.edu/outreach/googleearth/latlong.html
   DOUBLES_EQUAL(13170, point1.distanceInMeters(point2), 5);
}

TEST(ALocation, IsNotEqualToAnotherWhenLatDiffers) {
   Location point1{ 10, 11 };
   Location point2{ 11, 11 };

   CHECK_TRUE(point1 != point2);
}

TEST(ALocation, IsNotEqualToAnotherWhenLongDiffers) {
   Location point1{ 10, 11 };
   Location point2{ 10, 12 };

   CHECK_TRUE(point1 != point2);
}

TEST(ALocation, IsNotEqualToAnotherWhenLatAndLongMatch) {
   Location point1{ 10, 11 };
   Location point2{ 10, 11 };

   CHECK_TRUE(point1 == point2);
}

TEST(ALocation, AnswersNewLocationGivenDistanceAndBearing) {
   Location start{0, 0};
   
   auto newLocation = start.go(MetersPerDegreeAtEquator, East);

   Location expectedEnd{0, 1};
   DOUBLES_EQUAL(1, newLocation.longitude(), Tolerance);
   DOUBLES_EQUAL(0, newLocation.latitude(), Tolerance);
}

TEST(ALocation, AnswersNewLocationGivenDistanceAndBearingVerifiedByHaversine) {
   double distance{ 100 };
   Location start{ 38, -78 };

   auto end = start.go(distance, 35);

   DOUBLES_EQUAL(distance, start.distanceInMeters(end), Tolerance);
}

TEST(ALocation, CanBeAPole) {
   Location start{ 90, 0 };
   
   auto end = start.go(MetersPerDegreeAtEquator, South);

   DOUBLES_EQUAL(0, end.longitude(), Tolerance);
   DOUBLES_EQUAL(89, end.latitude(), Tolerance);
}

TEST(ALocation, IsVeryCloseToAnotherWhenSmallDistanceApart) {
   Location threeMetersAway { ArbitraryLocation.go(3, South) };

   CHECK_TRUE(ArbitraryLocation.isVeryCloseTo(threeMetersAway));
}

TEST(ALocation, IsNotVeryCloseToAnotherWhenNotSmallDistanceApart) {
   Location fourMetersAway { ArbitraryLocation.go(4, South) };

   CHECK_FALSE(ArbitraryLocation.isVeryCloseTo(fourMetersAway));
}

TEST(ALocation, ProvidesPrintableRepresentation) {
   Location location{-32, -105};
   stringstream s;

   s << location;

   CHECK_EQUAL("(-32,-105)", s.str());
}

