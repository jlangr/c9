#include "CppUTestExtensions.h"

SimpleString StringFrom(const std::vector<std::string>& list) {
   return StringFrom<std::string>(list, 
         [](std::string s) { return s; });
}

