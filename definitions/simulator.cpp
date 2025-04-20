#include "simulator.hpp"
namespace ParaCL
{
	Simulator::Simulator(const std::string& code)
	{
		Lexer lexer(code);
		lexer.tokenize();

		Parser parser(lexer.gettokens());
		parser.parse();

		root = parser.getAST();
	}

	bool Simulator::run()
	{
		return root->execute().value();
	}
}