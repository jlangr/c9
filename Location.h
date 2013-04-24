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
   Location();
   Location(double latitude, double longitude);

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

   double latitude() const;
   double longitude() const;

   bool operator==(const Location& that);
   bool operator!=(const Location& that);
   Location go(double meters, double bearing) const;
   double distanceInMeters(const Location& there) const;
   bool isUnknown() const;
   bool isVeryCloseTo(const Location& there) const;

private:
   double latitude_;
   double longitude_;

   double haversineDistance(Location there) const;
};

std::ostream& operator<<(std::ostream& output, const Location& location);

#endif
