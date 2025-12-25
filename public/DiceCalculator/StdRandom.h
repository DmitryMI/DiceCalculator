#pragma once

#include "DiceCalculator/IRandom.h"

#include <random>

namespace DiceCalculator
{
	class StdRandom : public IRandom
	{
	public:
		StdRandom();

		virtual ~StdRandom() = default;
		int NextInt(int minInclusive, int maxInclusive) override;

	private:
		std::random_device m_RandomDevice;
		std::mt19937 m_Generator;
	};
}