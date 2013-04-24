#include "Location.h"

#include <ostream>

using namespace std;

ostream& operator<<(ostream& output, const Location& location) {
   output << "(" << location.latitude() << "," << location.longitude() << ")";
   return output;
}
