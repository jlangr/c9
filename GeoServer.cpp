#include "GeoServer.h"
#include "Location.h"

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
   if (!isTracking(user)) return Location{}; // TODO performance cost?

   return find(user)->second;
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

vector<User> GeoServer::usersInBox(
      const string& user, double widthInMeters, double heightInMeters) const {
   auto location = locations_.find(user)->second;
   Area box { location, widthInMeters, heightInMeters };

   vector<User> users;
   for (auto& each: locations_) 
      if (isDifferentUserInBounds(each, user, box))
         users.push_back(User{each.first, each.second});
   return users;
}

