// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

//#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <numeric>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>
#include <chrono>
#include <array>
#include "windows.h"
#include "psapi.h"
#include <UIlib.h>
#include "CppUnitTest.h"

using namespace DuiLib;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "./StringUtils.h"
#include "../DuiLib/Utils/PathUtils.h"

#endif //PCH_H
