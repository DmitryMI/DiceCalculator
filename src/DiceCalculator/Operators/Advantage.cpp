#include "DiceCalculator/Operators/Advantage.h"
#include <stdexcept>

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

		operands[0]->Accept(visitor);
		int result1 = visitor.GetResult();
		operands[0]->Accept(visitor);
		int result2 = visitor.GetResult();
		if (m_Mode == Mode::Advantage)
		{
			return std::max(result1, result2);
		}
		else
		{
			return std::min(result1, result2);
		}
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

		if (m_Mode == Mode::Advantage)
		{
			// P_max(k) = F(k)^2 - F(k-1)^2, where F is CDF
			double cumulative = 0.0;
			for (auto const& p : d.GetData())
			{
				int k = p.first;
				double pk = p.second;
				double Fk = cumulative + pk;
				double Fprev = cumulative;
				double prob = Fk * Fk - Fprev * Fprev;
				result.AddOutcome(k, prob);
				cumulative = Fk;
			}
		}
		else
		{
			// Disadvantage (minimum): P_min(k) = S(k)^2 - S(k+1)^2, where S(k)=P(X>=k) is suffix CDF.
			double suffix = 0.0;
			auto const& data = d.GetData();
			for (auto it = data.rbegin(); it != data.rend(); ++it)
			{
				int k = it->first;
				double pk = it->second;
				double Sk = suffix + pk;        // P(X >= k)
				double Snext = suffix;         // P(X >= k+1)
				double prob = Sk * Sk - Snext * Snext;
				result.AddOutcome(k, prob);
				suffix = Sk;
			}
		}

		return result;
	}
}