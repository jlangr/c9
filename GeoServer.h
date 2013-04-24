#ifndef GeoServer_h
#define GeoServer_h

#include <string>
#include <unordered_map>
#include <vector>

#include "Location.h"
#include "Area.h"
#include "User.h"

class GeoServer {
public:
   void track(const std::string& user);
   void stopTracking(const std::string& user);
   void updateLocation(const std::string& user, const Location& location);

   bool isTracking(const std::string& user) const;
   Location locationOf(const std::string& user) const;

   bool isDifferentUserInBounds(
         const std::pair<std::string, Location>& each,
         const std::string& user,
         const Area& box) const {
      if (each.first == user) return false;
      return box.inBounds(each.second);
   }

   std::vector<User> usersInBox(const std::string& user, double widthInMeters, double heightInMeters) const {
      Location location = locations_.find(user)->second;
      Area box { location, widthInMeters, heightInMeters };

      std::vector<User> users;
      for (auto& each: locations_) 
         if (isDifferentUserInBounds(each, user, box))
            users.push_back(User{each.first, each.second});
      return users;
   }

private:
   std::unordered_map<std::string, Location> locations_;

   std::unordered_map<std::string, Location>::const_iterator 
      find(const std::string& user) const;
};

#endif
