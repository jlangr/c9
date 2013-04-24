#include "CppUTest/TestHarness.h"

#include "Area.h"
#include "Location.h"

TEST_GROUP(AnArea) {
   const double Tolerance { 0.005 };
   const Location Zero { 0, 0 };
   const double OneKm { 1000 };
   const double HalfOneKm { 500 };
   const double TwoKm { 2000 };
   const double HalfTwoKm { 1000 };
   const Area OneByTwoAreaAtZero{Zero, OneKm, TwoKm};
};

TEST(AnArea, AnswersUpperLeftForBoundingRectangle) {
   Location upperLeft{ OneByTwoAreaAtZero.upperLeft() };

   DOUBLES_EQUAL(Zero.go(HalfOneKm, West).longitude(), upperLeft.longitude(), Tolerance);
   DOUBLES_EQUAL(Zero.go(HalfTwoKm, North).latitude(), upperLeft.latitude(), Tolerance);
}

TEST(AnArea, AnswersUpperRightForBoundingRectangle) {
   Location upperRight{ OneByTwoAreaAtZero.upperRight() };

   DOUBLES_EQUAL(Zero.go(HalfOneKm, East).longitude(), upperRight.longitude(), Tolerance);
   DOUBLES_EQUAL(Zero.go(HalfTwoKm, North).latitude(), upperRight.latitude(), Tolerance);
}

TEST(AnArea, AnswersLowerRightForBoundingRectangle) {
   Location lowerRight{ OneByTwoAreaAtZero.lowerRight() };

   DOUBLES_EQUAL(Zero.go(HalfOneKm, East).longitude(), lowerRight.longitude(), Tolerance);
   DOUBLES_EQUAL(Zero.go(HalfTwoKm, South).latitude(), lowerRight.latitude(), Tolerance);
}

TEST(AnArea, AnswersLowerLeftForBoundingRectangle) {
   Location lowerLeft{ OneByTwoAreaAtZero.lowerLeft() };

   DOUBLES_EQUAL(Zero.go(HalfOneKm, West).longitude(), lowerLeft.longitude(), Tolerance);
   DOUBLES_EQUAL(Zero.go(HalfTwoKm, South).latitude(), lowerLeft.latitude(), Tolerance);
}

TEST(AnArea, DesignatesAPointOutOfBoundsWhenTooFarNorth) {
   Location tooNorth { Zero.go(HalfTwoKm + 10, North).latitude(), 0 };

   CHECK_FALSE(OneByTwoAreaAtZero.inBounds(tooNorth));
}

TEST(AnArea, DesignatesAPointInBoundsWhenAtNorth) {
   Location northEdge { Zero.go(HalfTwoKm, North).latitude(), 0 };

   CHECK_TRUE(OneByTwoAreaAtZero.inBounds(northEdge));
}

TEST(AnArea, DesignatesAPointOutOfBoundsWhenTooFarSouth) {
   Location tooSouth { Zero.go(HalfTwoKm + 10, South).latitude(), 0 };

   CHECK_FALSE(OneByTwoAreaAtZero.inBounds(tooSouth));
}

TEST(AnArea, DesignatesAPointInBoundsWhenAtSouth) {
   Location southEdge { Zero.go(HalfTwoKm, South).latitude(), 0 };

   CHECK_TRUE(OneByTwoAreaAtZero.inBounds(southEdge));
}

TEST(AnArea, DesignatesAPointOutOfBoundsWhenTooFarWest) {
   Location tooWest { 0, Zero.go(HalfOneKm + 10, West).longitude() };

   CHECK_FALSE(OneByTwoAreaAtZero.inBounds(tooWest));
}

TEST(AnArea, DesignatesAPointOutOfBoundsWhenTooFarEast) {
   Location tooEast { 0, Zero.go(HalfOneKm + 10, East).longitude() };

   CHECK_FALSE(OneByTwoAreaAtZero.inBounds(tooEast));
}

TEST(AnArea, DesignatesAPointInBoundsWhenAtWest) {
   Location westEdge { 0, Zero.go(HalfOneKm, West).longitude() };

   CHECK_TRUE(OneByTwoAreaAtZero.inBounds(westEdge));
}

TEST(AnArea, DesignatesAPointInBoundsWhenAtEast) {
   Location eastEdge { 0, Zero.go(HalfOneKm, East).longitude() };

   CHECK_TRUE(OneByTwoAreaAtZero.inBounds(eastEdge));
}

