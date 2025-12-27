#pragma once

#include <QWidget>
#include "ui_DiceExpressionInput.h"

namespace DiceCalculator::Ui::Widgets
{
	class DiceExpressionInput : public QWidget
	{
		Q_OBJECT

	public:
		enum class MessageType
		{
			Info,
			Warning,
			Error
		};

		enum class EvaluationMethod
		{
			Auto,
			Convolution,
			Combinatorial,
			Roll,
		};

		explicit DiceExpressionInput(QWidget* parent = nullptr);
		~DiceExpressionInput() override;

		[[nodiscard]] QString GetExpression() const;
		void SetExpression(const QString& value);

		void ClearMessages();
		void AppendMessage(const QString& message, MessageType type);

		EvaluationMethod GetEvaluationMethod() const
		{
			return static_cast<EvaluationMethod>(m_Ui.evaluationMethodBox->currentData().toInt());
		}

	signals:
		void ExpressionChanged(const QString& value);
		void ExpressionEvaluationRequested(const QString& expression);

	protected:
		void changeEvent(QEvent* event) override;

	private:
		Ui_DiceExpressionInput m_Ui;
	};
}