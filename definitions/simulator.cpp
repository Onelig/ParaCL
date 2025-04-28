#include <simulator.hpp>
#include <LanguageErrors.hpp>

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
	
	void Simulator::run()
	{
		root->execute();
	}
}