#ifndef CppUTestExtensions_h
#define CppUTestExtensions_h

#include <string>
#include <vector>
#include <sstream>
#include <functional>

#include "CppUTest/TestHarness.h"

template<typename T>
SimpleString StringFrom(const std::vector<T>& list, std::function<std::string(T)> func) {
   std::stringstream stream;
   for (auto each: list) {
      if (stream.str().length() > 0) stream << ",";
      stream << func(each);
   }
   return SimpleString(stream.str().c_str());
}

SimpleString StringFrom(const std::vector<std::string>& list);

#endif

