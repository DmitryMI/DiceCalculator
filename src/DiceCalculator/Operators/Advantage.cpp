#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include <stdexcept>
#include <cmath>

namespace DiceCalculator::Operators
{
	bool Advantage::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.size() == 1)
		{
			return true;
		}
		if(operands.size() == 2)
		{
			auto const& secondOperand = operands[1];
			auto const* constNode = dynamic_cast<DiceCalculator::Expressions::ConstantNode*>(secondOperand.get());
			return constNode;
		}
		return false;
	}

	int Advantage::Evaluate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.size() > 2)
		{
			throw std::runtime_error("Too many arguments for Advantage()");
		}

		int rerolls = GetRerolls(operands);
		
		std::vector<int> rolledValues;
		std::vector<std::vector<Evaluation::RollAstVisitor::DiceRollRecord>> rollRecords;

		for (int i = 0; i < rerolls; ++i)
		{
			operands[0]->Accept(visitor);
			int result = visitor.GetResult();
			auto records = visitor.GetDiceRecords();
			rolledValues.push_back( result );
			rollRecords.push_back(records);
		}

		if (rolledValues.empty())
		{
			throw std::runtime_error("Advantage operator performed no rolls.");
		}

		// Find index of max (advantage) or min (disadvantage)
		std::size_t bestIndex = 0;
		for (std::size_t i = 1; i < rolledValues.size(); ++i)
		{
			if (m_Mode == Mode::Advantage)
			{
				if (rolledValues[i] > rolledValues[bestIndex])
				{
					bestIndex = i;
				}
			}
			else
			{
				if (rolledValues[i] < rolledValues[bestIndex])
				{
					bestIndex = i;
				}
			}
		}

		visitor.SetDiceRecords(rollRecords[bestIndex]);
		return rolledValues[bestIndex];
	}

	Distribution Advantage::Evaluate(DiceCalculator::Evaluation::ConvolutionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.size() > 2)
		{
			throw std::runtime_error("Too many arguments for Advantage()");
		}

		int rerolls = GetRerolls(operands);

		// Evaluate operand distribution (single-draw distribution)
		operands[0]->Accept(visitor);
		const Distribution& d = visitor.GetDistribution();

		if (d.Size() == 0)
		{
			return Distribution();
		}

		Distribution result;

		// Use m_Rerolls (n draws) in formulas:
		// P_max(k) = F(k)^n - F(k-1)^n
		// P_min(k) = S(k)^n - S(k+1)^n
		const double n = static_cast<double>(std::max(1, rerolls));

		if (m_Mode == Mode::Advantage)
		{
			double cumulative = 0.0;
			for (auto const& p : d.GetData())
			{
				int k = p.first;
				double pk = p.second;
				double Fk = cumulative + pk;
				double Fprev = cumulative;
				double prob = std::pow(Fk, n) - std::pow(Fprev, n);
				result.AddOutcome(k, prob);
				cumulative = Fk;
			}
		}
		else
		{
			double suffix = 0.0;
			auto const& data = d.GetData();
			for (auto it = data.rbegin(); it != data.rend(); ++it)
			{
				int k = it->first;
				double pk = it->second;
				double Sk = suffix + pk;        // P(X >= k)
				double Snext = suffix;         // P(X >= k+1)
				double prob = std::pow(Sk, n) - std::pow(Snext, n);
				result.AddOutcome(k, prob);
				suffix = Sk;
			}
		}

		return result;
	}

	std::vector<Combination> Advantage::Evaluate(DiceCalculator::Evaluation::CombinationAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.size() > 2)
		{
			throw std::runtime_error("Too many arguments for Advantage()");
		}

		int rerolls = GetRerolls(operands);

		// Get all combinations for a single roll of the operand.
		operands[0]->Accept(visitor);
		const auto& baseCombos = visitor.GetCombinations();
		if (baseCombos.empty())
		{
			return {};
		}

		const int n = std::max(1, rerolls);
		if (n == 1)
		{
			// Only one roll, just pass-through.
			return baseCombos;
		}

		// Build cartesian products of 'n' independent rolls of the same operand.
		// For each product, select the best attempt according to mode and record ONLY that attempt's rolls.
		std::vector<Combination> result;
		// total count = baseCombos.size() ^ n
		// We iterate using an index vector like mixed-radix odometer.
		const size_t m = baseCombos.size();
		std::vector<size_t> indices(static_cast<size_t>(n), 0);

		// Helper to push current product's selected attempt
		auto emitCurrent = [&]()
		{
			// Find best attempt index among current indices
			size_t bestIdxInProduct = 0;
			for (size_t i = 1; i < indices.size(); ++i)
			{
				const auto& cur = baseCombos[indices[i]].TotalValue;
				const auto& best = baseCombos[indices[bestIdxInProduct]].TotalValue;
				if (m_Mode == Mode::Advantage)
				{
					if (cur > best) bestIdxInProduct = i;
				}
				else
				{
					if (cur < best) bestIdxInProduct = i;
				}
			}

			const auto& chosen = baseCombos[indices[bestIdxInProduct]];
			Combination out;
			out.TotalValue = chosen.TotalValue;
			out.Rolls = chosen.Rolls; // ONLY record the chosen attempt's rolls
			result.push_back(std::move(out));
			if (result.size() > Evaluation::CombinationAstVisitor::MaxCombinationsThreshold)
			{
				throw std::runtime_error("Combination evaluation exceeded maximum allowed combinations.");
			}
		};

		// Odometer iteration over m^n combinations
		bool done = false;
		while (!done)
		{
			emitCurrent();

			// increment indices
			for (size_t pos = 0; pos < indices.size(); ++pos)
			{
				if (++indices[pos] < m)
				{
					break; // carry resolved
				}
				indices[pos] = 0; // reset and carry to next
				if (pos + 1 == indices.size())
				{
					done = true;
				}
			}
		}

		return result;
	}

	int Advantage::GetRerolls(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		int rerolls = 2; // default
		if (operands.size() == 2)
		{
			auto const& secondOperand = operands[1];
			auto const* constNode = dynamic_cast<DiceCalculator::Expressions::ConstantNode*>(secondOperand.get());
			if (!constNode)
			{
				throw std::runtime_error("Reroll count for Advantage() must be a constant.");
			}
			rerolls = constNode->GetValue();
			if (rerolls < 1)
			{
				throw std::runtime_error("Reroll count for Advantage() must be at least 1.");
			}
		}
		return rerolls;
	}

	bool Advantage::IsEqual(const DiceOperator& other) const
	{
		if (const auto* otherAdv = dynamic_cast<const Advantage*>(&other))
		{
			return m_Mode == otherAdv->m_Mode;
		}
		return false;
	}

	std::vector<RegistryEntry> Advantage::Register()
	{
		return {
			{ RegistryEntry{"ADV", Arity::Function, []() { return std::make_shared<Advantage>(Advantage::Mode::Advantage); }} },
			{ RegistryEntry{"DIS", Arity::Function, []() { return std::make_shared<Advantage>(Advantage::Mode::Disadvantage); }} }
		};
	}
}