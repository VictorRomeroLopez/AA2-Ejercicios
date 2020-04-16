#include "Console.h"

Console::ConsoleCommand Console::isCommand(std::string command) {

	if (command == "exit") return ConsoleCommand::EXIT;
	return ConsoleCommand::NONE;

}

void Console::console(bool &running) {

	ConsoleCommand command = ConsoleCommand::NONE;
	std::string inputConsole = "";

	while (command != ConsoleCommand::EXIT) {
		std::cin >> inputConsole;
		switch (isCommand(inputConsole)) {

		case ConsoleCommand::EXIT:
			Utils::print("exit");
			running = false;
			return;

		default:
			Utils::print("The command " + inputConsole + " does not exist!");
		}
	}

}
