#pragma once

#include <QWidget>
#include "ui_DiceExpressionBlock.h"

namespace DiceCalculator::Ui::Widgets
{
	class DiceExpressionInput;

	class DiceExpressionBlock : public QWidget
	{
		Q_OBJECT

	public:
		explicit DiceExpressionBlock(QWidget* parent = nullptr);
		~DiceExpressionBlock() override;

	signals:
		void RemoveRequested(DiceExpressionBlock* block);

	private:
		Ui_DiceExpressionBlock m_Ui;
		DiceExpressionInput* m_DiceExpressionInput{ nullptr };
	};
}