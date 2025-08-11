#pragma once

#include <parser.hpp>

namespace ParaCL
{
	class Simulator
	{
		std::unique_ptr<Node> root;

	public:
		Simulator() = default;
		Simulator(const std::string& code);
		void set(const std::string& code);
		void run();
	};
}