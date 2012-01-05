#pragma once

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "detours.lib")

#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "Types.h"

#include "IAlgorithm.h"
#include "IPatternAlgorithm.h"
#include "BoyerMooreHorspool.h"

#include "detours.h"
#include "NoMagic.h"
