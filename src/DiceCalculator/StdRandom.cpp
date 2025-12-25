#pragma once

#include "DiceCalculator/StdRandom.h"

namespace DiceCalculator
{
	StdRandom::StdRandom() : m_Generator(m_RandomDevice())
	{
	}

	int StdRandom::NextInt(int minInclusive, int maxInclusive)
	{
		std::uniform_int_distribution<> distrib(minInclusive, maxInclusive);
		return distrib(m_Generator);
	}
}