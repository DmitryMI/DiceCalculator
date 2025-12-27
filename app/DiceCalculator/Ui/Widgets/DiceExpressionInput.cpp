#pragma once

#include "DiceCalculator/Ui/Widgets/DiceExpressionInput.h"
#include <QEvent>
#include <qpushbutton.h>

namespace DiceCalculator::Ui::Widgets
{
	DiceExpressionInput::DiceExpressionInput(QWidget* parent)
		: QWidget(parent)
	{
		m_Ui.setupUi(this);

		m_Ui.evaluationMethodBox->addItem("Auto", static_cast<int>(EvaluationMethod::Auto));
		m_Ui.evaluationMethodBox->addItem("Convolution", static_cast<int>(EvaluationMethod::Convolution));
		m_Ui.evaluationMethodBox->addItem("Combinatorial", static_cast<int>(EvaluationMethod::Combinatorial));
		m_Ui.evaluationMethodBox->addItem("Roll", static_cast<int>(EvaluationMethod::Roll));
		m_Ui.evaluationMethodBox->setCurrentIndex(0);

		connect(m_Ui.expressionEdit, &QTextEdit::textChanged, this, [this](){emit ExpressionChanged(m_Ui.expressionEdit->toPlainText());});
		connect(m_Ui.evaluateButton, &QPushButton::clicked, this, [this]() {emit ExpressionEvaluationRequested(m_Ui.expressionEdit->toPlainText()); });
	}

	DiceExpressionInput::~DiceExpressionInput() = default;

	QString DiceExpressionInput::GetExpression() const
	{
		return m_Ui.expressionEdit->toPlainText();
	}

	void DiceExpressionInput::SetExpression(const QString& value)
	{
		m_Ui.expressionEdit->setPlainText(value);
	}

	void DiceExpressionInput::ClearMessages()
	{
	}

	void DiceExpressionInput::AppendMessage(const QString& message, MessageType type)
	{
	}

	void DiceExpressionInput::changeEvent(QEvent* event)
	{
		QWidget::changeEvent(event);
		if (event && event->type() == QEvent::LanguageChange)
		{
			m_Ui.retranslateUi(this);
		}
	}
}