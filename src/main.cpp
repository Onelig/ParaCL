#include <fstream>
#include <signal.h>

#include <simulator.hpp>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#define NOMINMAX
#include <limits>
#endif 

bool forever = true;

void sighandler(int sig)
{
	forever = false;
}

int main()
{
	std::unique_ptr<int> obj(new int(2));
	signal(SIGINT, &sighandler);
	std::string code;
	
	char type_read = NULL;
#ifdef _WIN32
	std::cout << "Choose option(press key):" << std::endl;
#else
	std::cout << "Choose option(enter key):" << std::endl;
#endif

	std::cout << "1. Write code via console" << std::endl;
	std::cout << "2. Select already exist written code" << std::endl;

#ifdef _WIN32
	while (!type_read)
	{
		if (GetKeyState(0x31)) // 1
			type_read = '1';

		else if (GetKeyState(0x32)) // 2
			type_read = '2';
	}

	system("cls");
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
#else
	std::cin >> type_read;
	while (std::cin.fail() || (type_read != '1' && type_read != '2'))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
		std::cout << "\033[A\033[K"; // clear

		std::cin >> type_read;
	};
	system("clear");
#endif
	if (type_read == '1')
	{
		while (forever)
		{
			std::string line;
			std::getline(std::cin, line);

			if (std::cin.good())
				code += line + '\n';
		}
	}
	else if (type_read == '2')
	{
		std::string path;
		std::cout << "Enter path: ";
		std::getline(std::cin, path);

		std::ifstream f(path);
		while (!f.is_open())
		{
			std::cout << "Incorrect path. Enter path: ";
			std::getline(std::cin, path);
			f.open(path);
		}
		
		while (std::getline(f, path))
		{
			code += path + '\n';
		}
	}

	try
	{	
		ParaCL::Simulator simulator(code);
		simulator.run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	
	return 0;
}