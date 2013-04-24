#include "TestTimer.h"

#include "CppUTest/Utest.h"
#include <iostream>

using namespace std;

TestTimer::TestTimer()
   : TestTimer(UtestShell::getCurrent()->getName().asCharString()) {
}

TestTimer::TestTimer(const string& text) 
   : Start{chrono::system_clock::now()} 
   , Text{text} {}

TestTimer::~TestTimer() {
   Stop = chrono::system_clock::now();
   Elapsed = chrono::duration_cast<chrono::microseconds>(Stop - Start);
   cout << endl <<
      Text << " elapsed time = " << Elapsed.count() * 0.001 << "ms" << endl;
}

