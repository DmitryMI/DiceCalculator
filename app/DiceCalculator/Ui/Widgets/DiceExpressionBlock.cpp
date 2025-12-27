#include "DiceCalculator/Ui/Widgets/DiceExpressionBlock.h"
#include "DiceCalculator/Ui/Widgets/DiceExpressionInput.h"

namespace DiceCalculator::Ui::Widgets
{
	DiceExpressionBlock::DiceExpressionBlock(QWidget* parent)
		: QWidget(parent)
	{
		m_Ui.setupUi(this);

		m_DiceExpressionInput = new DiceExpressionInput(this);
		m_DiceExpressionInput->setSizePolicy(m_Ui.diceExpressionInputPlaceholder->sizePolicy());
		m_DiceExpressionInput->setMinimumSize(m_Ui.diceExpressionInputPlaceholder->minimumSize());
		m_Ui.gridLayout_2->replaceWidget(m_Ui.diceExpressionInputPlaceholder, m_DiceExpressionInput);
		m_Ui.diceExpressionInputPlaceholder->deleteLater();
		m_Ui.diceExpressionInputPlaceholder = nullptr;

		connect(m_Ui.removeBlockButton, &QPushButton::clicked, this, [this]() {
			emit RemoveRequested(this);
			});
	}

	DiceExpressionBlock::~DiceExpressionBlock() = default;
}