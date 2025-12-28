#pragma once

#include <QWidget>
#include "ui_DiceExpressionInput.h"
#include "DiceCalculator/Controllers/ExpressionEvaluationController.h"

namespace DiceCalculator::Ui::Widgets
{
	class DiceExpressionInput : public QWidget
	{
		Q_OBJECT

	public:
		

		explicit DiceExpressionInput(QWidget* parent = nullptr);
		~DiceExpressionInput() override;

		[[nodiscard]] QString GetExpression() const;
		Controllers::ExpressionEvaluationController::EvaluationMethod GetExpressionEvaluationMethod() const;

		void SetExpression(const QString& value);

		void ClearMessages();
		void AppendMessage(const QString& message, Controllers::ExpressionEvaluationController::MessageType type);

		Controllers::ExpressionEvaluationController::EvaluationMethod GetEvaluationMethod() const
		{
			return static_cast<Controllers::ExpressionEvaluationController::EvaluationMethod>(m_Ui.evaluationMethodBox->currentData().toInt());
		}


	signals:
		void ExpressionChanged(const QString& value);
		void ExpressionEvaluationRequested(const QString& expression, Controllers::ExpressionEvaluationController::EvaluationMethod method);

	protected:
		void changeEvent(QEvent* event) override;

	private:
		Ui_DiceExpressionInput m_Ui;
	};
}