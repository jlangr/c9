#include "GeoServer.h"
#include "Location.h"


#include <iostream>

using namespace std;

void GeoServer::track(const string& user) {
   locations_[user] = Location();
}

void GeoServer::stopTracking(const string& user) {
   locations_.erase(user);
}

bool GeoServer::isTracking(const string& user) const {
   return find(user) != locations_.end();
}

void GeoServer::updateLocation(const string& user, const Location& location) {
   locations_[user] = location;
}

Location GeoServer::locationOf(const string& user) const {
   // optimized
   auto it = find(user);
   if (it == locations_.end()) return Location{};
   return it->second;
}

Location GeoServer::virtualLocationOf(const string& user) const {
   // optimized
   auto it = find(user);
   if (it == locations_.end()) return Location{};
   return it->second;
}

std::unordered_map<std::string, Location>::const_iterator 
   GeoServer::find(const std::string& user) const {
   return locations_.find(user);
}

bool GeoServer::isDifferentUserInBounds(
      const pair<string, Location>& each,
      const string& user,
      const Area& box) const {
   if (each.first == user) return false;
   return box.inBounds(each.second);
}

void GeoServer::usersInBox(
      const string& user, double widthInMeters, double heightInMeters,
      GeoServerListener* listener) const {
   auto location = locations_.find(user)->second;
   Area box { location, widthInMeters, heightInMeters };

   for (auto& each: locations_) {
      Work work{[&] {
         if (isDifferentUserInBounds(each, user, box)) 
            listener->updated(User{each.first, each.second});
      }};
      pool_->add(work);
   }
}

void GeoServer::useThreadPool(std::shared_ptr<ThreadPool> pool) {
   pool_ = pool;
}

