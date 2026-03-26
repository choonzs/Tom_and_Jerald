// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_HPP
#define PCH_HPP

//Basically, just put libraries you want to pre-compile in here, not like code that you are changing often


// add headers that you want to pre-compile here
//#include "GameStateList.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <AEMath.h>
#include <crtdbg.h> // To check for memory leaks
#include <cmath>
#include <ctime>
#include "AEEngine.h"
#include <cstdlib>
#include <vector>
#include <iomanip>
#include <filesystem> // For file system operations like checking if file exists, creating directories, etc.

#include <list> //To use initializer list in particles.cpp
#include <map>	//Particles.cpp
#include <array> //Array.cpp

#endif //PCH_H
