#pragma once

#include <parser.hpp>

namespace ParaCL
{
	class Simulator
	{
		std::unique_ptr<Node> root;

	public:
		Simulator(const std::string& code);
		bool run();
	};

}