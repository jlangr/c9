#ifndef Location_h
#define Location_h

#include <limits>
#include <cmath>
#include <ostream>

const double Pi{ 4.0 * atan(1.0) };
const double ToRadiansConversionFactor{ Pi / 180 };
const double RadiusOfEarthInMeters{ 6372000 };
const double MetersPerDegreeAtEquator{ 111111 };
const double North{ 0 };
const double West{ 90 };
const double South{ 180 };
const double East{ 270 };
const double CloseMeters{ 3 };

class Location {
public:
   Location() 
      : latitude_(std::numeric_limits<double>::infinity())
      , longitude_(std::numeric_limits<double>::infinity()) {}

   Location(double latitude, double longitude) 
      : latitude_(latitude), longitude_(longitude) {}

   double latitude() const {
      return latitude_;
   }

   double longitude() const {
      return longitude_;
   }

   inline double toRadians(double degrees) const {
      return degrees * ToRadiansConversionFactor;
   }

   inline double toCoordinate(double radians) const {
      return radians * (180 / Pi);
   }

   inline double latitudeAsRadians() const {
      return toRadians(latitude_);
   }

   inline double longitudeAsRadians() const {
      return toRadians(longitude_);
   }

   bool operator==(const Location& that) {
      return 
         longitude_ == that.longitude_ &&
         latitude_ == that.latitude_;
   }

   bool operator!=(const Location& that) {
      return !(*this == that);
   }

   // from williams.best.vwh.net/avform.htm#LL
   Location go(double meters, double bearing) const {
      bearing = toRadians(bearing);
      double distance { meters / RadiusOfEarthInMeters };
      double newLat { 
         asin(sin(latitudeAsRadians()) * cos(distance) + 
              cos(latitudeAsRadians()) * sin(distance) * cos(bearing)) };

      double newLong = longitudeAsRadians();
      if (cos(latitudeAsRadians()) != 0) 
         newLong = 
            fmod(longitudeAsRadians() - asin(sin(bearing) * sin(distance) / cos(newLat)) + Pi,
                 2 * Pi) - Pi;

      return Location(toCoordinate(newLat), toCoordinate(newLong));
   }

   double distanceInMeters(const Location& there) const {
      return RadiusOfEarthInMeters * haversineDistance(there);
   }

   bool isUnknown() const {
      return latitude_ == std::numeric_limits<double>::infinity();
   }

   bool isVeryCloseTo(const Location& there) const {
      return distanceInMeters(there) <= CloseMeters;
   }

private:
   double latitude_;
   double longitude_;

   double haversineDistance(Location there) const {
      double deltaLongitude { longitudeAsRadians() - there.longitudeAsRadians() };
      double deltaLatitude { latitudeAsRadians() - there.latitudeAsRadians() };

      double aHaversine { 
         pow(
            sin(deltaLatitude / 2.0), 2.0) + 
               cos(latitudeAsRadians()) * cos(there.latitudeAsRadians()) * pow(sin(deltaLongitude / 2), 
            2) };
      return 2 * atan2(sqrt(aHaversine), sqrt(1.0 - aHaversine));
   }
};

// TODO write test!
std::ostream& operator<<(std::ostream& output, const Location& location);

#endif
