#include <lexer.hpp>
#include <fstream>
#include <iostream>
#include <signal.h>

bool forever = true;

void sighandler(int sig)
{
	forever = false;
}

int main()
{
	signal(SIGINT, &sighandler);
	std::string code;
	while (forever)
	{
		std::string line;
		std::getline(std::cin, line);

		if (std::cin.good())
			code += line + ' ';
	}

	return 0;
}