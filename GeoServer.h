#ifndef GeoServer_h
#define GeoServer_h

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

#include "Location.h"
#include "Area.h"
#include "User.h"
#include "ThreadPool.h"

class GeoServerListener {
public:
   virtual void updated(const User& user)=0;
};

class GeoServerBase {
public:
   virtual Location virtualLocationOf(const std::string& user) const {
      throw 1;
   }
};

class GeoServer: public GeoServerBase {
public:
   virtual ~GeoServer() {}
   void track(const std::string& user);
   void stopTracking(const std::string& user);
   void updateLocation(const std::string& user, const Location& location);

   bool isTracking(const std::string& user) const;
// START:impl
   virtual Location virtualLocationOf(const std::string& user) 
      const override
      __attribute__((noinline));
// END:impl
   Location locationOf(const std::string& user) const;
   bool isDifferentUserInBounds(
         const std::pair<std::string, Location>& each,
         const std::string& user,
         const Area& box) const;
   void usersInBox(
         const std::string& user, double widthInMeters, double heightInMeters,
         GeoServerListener* listener) const;
   void useThreadPool(std::shared_ptr<ThreadPool> pool);

private:
   std::unordered_map<std::string, Location> locations_;

   std::unordered_map<std::string, Location>::const_iterator 
      find(const std::string& user) 
      const
      __attribute__((noinline));
   std::shared_ptr<ThreadPool> pool_;
   unsigned int i{0};
};
#endif
