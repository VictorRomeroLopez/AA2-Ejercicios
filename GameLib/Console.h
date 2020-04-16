#pragma once
#include <iostream>
#include <string>

#include "Utils.h"

class Console {

public:
	enum class ConsoleCommand {
		EXIT,
		NONE
	};

	static ConsoleCommand isCommand(std::string command);
	static void console(bool &running);

};