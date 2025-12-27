#include "DiceCalculator/Operators/Advantage.h"
#include <stdexcept>
#include <cmath>

namespace DiceCalculator::Operators
{
	bool Advantage::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		return operands.size() == 1;
	}

	int Advantage::Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (!Validate(operands))
		{
			throw std::runtime_error("Advantage operator requires exactly one operand.");
		}

		std::vector<int> rolledValues;
		std::vector<std::vector<Evaluation::RollAstVisitor::DiceRollRecord>> rollRecords;

		for (int i = 0; i < m_Rerolls; ++i)
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

	Distribution Advantage::Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (!Validate(operands))
		{
			throw std::runtime_error("Advantage operator requires exactly one operand.");
		}

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
		const double n = static_cast<double>(std::max(1, m_Rerolls));

		if (m_Mode == Mode::Advantage)
		{
			double cumulative = 0.0;
			for (auto const& p : d.GetData())
			{
				int k = p.Value;
				double pk = p.Probability;
				double Fk = cumulative + pk;
				double Fprev = cumulative;
				double prob = std::pow(Fk, n) - std::pow(Fprev, n);
				result.AddOutcome(k, prob, p.D20);
				cumulative = Fk;
			}
		}
		else
		{
			double suffix = 0.0;
			auto const& data = d.GetData();
			for (auto it = data.rbegin(); it != data.rend(); ++it)
			{
				int k = it->Value;
				double pk = it->Probability;
				double Sk = suffix + pk;        // P(X >= k)
				double Snext = suffix;         // P(X >= k+1)
				double prob = std::pow(Sk, n) - std::pow(Snext, n);
				result.AddOutcome(k, prob, it->D20);
				suffix = Sk;
			}
		}

		return result;
	}

	bool Advantage::IsEqual(const DiceOperator& other) const
	{
		if (const auto* otherAdv = dynamic_cast<const Advantage*>(&other))
		{
			return m_Mode == otherAdv->m_Mode && m_Rerolls == otherAdv->m_Rerolls;
		}
		return false;
	}

	std::vector<OperatorRegistry::Entry> Advantage::Register()
	{
		registered = true;
		return {
			{ OperatorRegistry::Entry{"ADV", OperatorRegistry::Arity::Function, []() { return std::make_shared<Advantage>(Advantage::Mode::Advantage); }} },
			{ OperatorRegistry::Entry{"DIS", OperatorRegistry::Arity::Function, []() { return std::make_shared<Advantage>(Advantage::Mode::Disadvantage); }} }
		};
	}
}