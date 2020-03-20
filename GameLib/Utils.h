#pragma once
#include <iostream>
#include <string>
#include "Constants.h"
#include <time.h>

class Utils
{
public:
	static void print(std::string text); 
	static unsigned short GenerateRandomSalt();
	static unsigned short GenerateRandom(unsigned short);
};

