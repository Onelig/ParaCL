#include <fstream>
#include <signal.h>

#include <lexer.hpp>
#include <parser.hpp>

bool forever = true;

void sighandler(int sig)
{
	forever = false;
}

int main()
{
	Lexer obj("v = 100;");
	obj.tokenize();
	Parser parser(obj.gettokens());
	parser.parse();

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