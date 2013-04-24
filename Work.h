#ifndef Work_h
#define Work_h

#include <functional>

class Work {
public:
   Work(std::function<void()> executeFunction) 
      : executeFunction_(executeFunction) {}

   void execute() {
      executeFunction_();
   }

private:
   std::function<void()> executeFunction_;
};

#endif

