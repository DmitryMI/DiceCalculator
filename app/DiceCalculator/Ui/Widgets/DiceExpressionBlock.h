#pragma once

#include <QWidget>
#include <memory>
#include "ui_DiceExpressionBlock.h"
#include "DiceCalculator/Distribution.h"

namespace DiceCalculator::Controllers
{
	class ExpressionEvaluationController;
}

namespace DiceCalculator::Ui::Widgets
{
	class DiceExpressionInput;

	class DiceExpressionBlock : public QWidget
	{
		Q_OBJECT

	public:
		explicit DiceExpressionBlock(
			DiceCalculator::Controllers::ExpressionEvaluationController* controller,
			QWidget* parent = nullptr
		);
		~DiceExpressionBlock() override;

	signals:
		void RemoveRequested(DiceExpressionBlock* block);

	private:
		Ui_DiceExpressionBlock m_Ui;
		DiceExpressionInput* m_DiceExpressionInput{ nullptr };
		DiceCalculator::Controllers::ExpressionEvaluationController* m_Controller;

		void ClearOutput();
		void AddPlot(const QString& expression, const Distribution& distribution);
	};
}