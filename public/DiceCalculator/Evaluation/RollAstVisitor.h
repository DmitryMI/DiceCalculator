#pragma once

#include "DiceCalculator/Evaluation/DiceAstVisitor.h"
#include "DiceCalculator/Distribution.h"
#include "DiceCalculator/IRandom.h"

namespace DiceCalculator::Evaluation
{
	class RollAstVisitor : public Evaluation::DiceAstVisitor
	{
	public:
		struct DiceRollRecord
		{
			int Sides;
			int RolledValue;
		};

		RollAstVisitor(IRandom& random) : m_Random(random), m_Result(0) {}

		void Visit(const DiceCalculator::Expressions::ConstantNode& node) override;
		void Visit(const DiceCalculator::Expressions::DiceNode& node) override;
		void Visit(const DiceCalculator::Expressions::OperatorNode& node) override;

		int GetResult() const { return m_Result; }

		const std::vector<DiceRollRecord>& GetDiceRecords() const { return m_DiceRecords; }
		void SetDiceRecords(const std::vector<DiceRollRecord>& records)
		{
			m_DiceRecords = records;
		}

	private:
		int m_Result;
		IRandom& m_Random;

		/// <summary>
		/// Dice rolls
		/// </summary>
		std::vector<DiceRollRecord> m_DiceRecords;
	};
}