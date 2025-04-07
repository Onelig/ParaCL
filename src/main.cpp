#include <fstream>
#include <signal.h>

#include <simulator.hpp>

bool forever = true;

void sighandler(int sig)
{
	forever = false;
}

int main()
{
	try
	{
		signal(SIGINT, &sighandler);
		std::string code;
		while (forever)
		{
			std::string line;
			std::getline(std::cin, line);

			if (std::cin.good())
				code += line;
		}

		Simulator simulator(code);
		simulator.run();

	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	
	return 0;
}