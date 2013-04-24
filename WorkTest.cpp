#include "CppUTest/TestHarness.h"

#include "Work.h"

#include <functional>
#include <vector>
#include <sstream>

using namespace std;

TEST_GROUP(AWorkObject) {
};

TEST(AWorkObject, ExecutesFunctionStored) {
   bool wasExecuted{false};
   auto executeFunction = [&] () { wasExecuted = true; };
   Work work(executeFunction);
   
   work.execute();

   CHECK_TRUE(wasExecuted);
}

TEST(AWorkObject, CanExecuteOnDataCapturedWithFunction) {
   vector<string> data{"a", "b"};
   string result;
   auto callbackFunction = [&](string s) {
      result.append(s);
   };
   auto executeFunction = [&]() {
      stringstream s;
      s << data[0] << data[1];
      callbackFunction(s.str());
   };
   Work work(executeFunction);

   work.execute();

   CHECK_EQUAL("ab", result);
}

