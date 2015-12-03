#ifndef MOENJIN_PLATFORM_WIN_PLATFORM_H
#define MOENJIN_PLATFORM_WIN_PLATFORM_H

#define _USE_MATH_DEFINES

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <cmath>
#include <vector>
#include <map>
#include <stack>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>
#include <algorithm>
#include <limits>
#include <istream>
#include <ostream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <wchar.h>
#include <string>
#include <tchar.h>
#include <setjmp.h>

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

using std::min;
using std::max;

#include "Platform/Win/PlatformConfiguration.h"

#endif