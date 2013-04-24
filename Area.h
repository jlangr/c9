#ifndef Area_h
#define Area_h

#include "Location.h"

class Area {
public:
   Area(const Location& location, double width, double height);
   Location upperLeft() const;
   Location upperRight() const;
   Location lowerRight() const;
   Location lowerLeft() const;
   bool inBounds(const Location&) const;

private:
   double left_;
   double right_;
   double top_;
   double bottom_;
};

#endif
