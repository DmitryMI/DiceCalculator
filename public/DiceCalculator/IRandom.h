#pragma once

namespace DiceCalculator
{
	class IRandom
	{
	public:
		virtual int NextInt(int minInclusive, int maxInclusive) = 0;
	};
}