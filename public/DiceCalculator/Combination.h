#pragma once

#include <vector>
#include <initializer_list>

namespace DiceCalculator
{
	class Combination
	{
	public:

		struct Roll
		{
			int Sides = 0;
			int Value = 0;
		};

		int TotalValue = 0;
		std::vector<Roll> Rolls = {};
	};
}