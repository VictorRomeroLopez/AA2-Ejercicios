#include "Utils.h"

void Utils::print(std::string text)
{
	std::cout << text << std::endl;
}

std::string Utils::intToString(int intToParse) {
	return std::to_string(intToParse);
}
