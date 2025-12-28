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

		m_Ui.evaluationMethodBox->addItem("Auto", static_cast<int>(Controllers::ExpressionEvaluationController::EvaluationMethod::Auto));
		m_Ui.evaluationMethodBox->addItem("Convolution", static_cast<int>(Controllers::ExpressionEvaluationController::EvaluationMethod::Convolution));
		m_Ui.evaluationMethodBox->addItem("Combinatorial", static_cast<int>(Controllers::ExpressionEvaluationController::EvaluationMethod::Combinatorial));
		m_Ui.evaluationMethodBox->addItem("Roll", static_cast<int>(Controllers::ExpressionEvaluationController::EvaluationMethod::Roll));
		m_Ui.evaluationMethodBox->setCurrentIndex(0);

		connect(m_Ui.expressionEdit, &QTextEdit::textChanged, this, [this](){emit ExpressionChanged(m_Ui.expressionEdit->toPlainText());});
		connect(m_Ui.evaluateButton, &QPushButton::clicked, this, [this]() {emit ExpressionEvaluationRequested(m_Ui.expressionEdit->toPlainText(), GetExpressionEvaluationMethod()); });
	}

	DiceExpressionInput::~DiceExpressionInput() = default;

	QString DiceExpressionInput::GetExpression() const
	{
		return m_Ui.expressionEdit->toPlainText();
	}

	Controllers::ExpressionEvaluationController::EvaluationMethod DiceExpressionInput::GetExpressionEvaluationMethod() const
	{
		return m_Ui.evaluationMethodBox->currentData().value<Controllers::ExpressionEvaluationController::EvaluationMethod>();
	}

	void DiceExpressionInput::SetExpression(const QString& value)
	{
		m_Ui.expressionEdit->setPlainText(value);
	}

	void DiceExpressionInput::ClearMessages()
	{
		QVBoxLayout* messagesGroupLayout = qobject_cast<QVBoxLayout*>(m_Ui.messagesGroup->layout());
		if (!messagesGroupLayout)
		{
			messagesGroupLayout = new QVBoxLayout();
			m_Ui.messagesGroup->setLayout(messagesGroupLayout);
		}

		QLayoutItem* item;
		while ((item = messagesGroupLayout->takeAt(0)) != NULL)
		{
			delete item->widget();
			delete item;
		}
	}

	void DiceExpressionInput::AppendMessage(const QString& message, Controllers::ExpressionEvaluationController::MessageType type)
	{
		QVBoxLayout* messagesGroupLayout = qobject_cast<QVBoxLayout*>(m_Ui.messagesGroup->layout());
		if (!messagesGroupLayout)
		{
			messagesGroupLayout = new QVBoxLayout();
			m_Ui.messagesGroup->setLayout(messagesGroupLayout);
		}

		QLabel* messageLabel = new QLabel(message, this);
		messageLabel->setWordWrap(true);

		switch (type)
		{
		case Controllers::ExpressionEvaluationController::MessageType::Info:
			messageLabel->setStyleSheet("color: rgb(33, 150, 243);");
			break;
		case Controllers::ExpressionEvaluationController::MessageType::Warning:
			messageLabel->setStyleSheet("color: rgb(209, 127, 0);");
			break;
		case Controllers::ExpressionEvaluationController::MessageType::Error:
			messageLabel->setStyleSheet("color: rgb(198, 40, 40);");
			break;
		default:
			break;
		}

		messagesGroupLayout->addWidget(messageLabel);
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