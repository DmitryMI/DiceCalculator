
#include <QObject>
#include "DiceCalculator/Controllers/ExpressionEvaluationController.h"

namespace DiceCalculator::Controllers
{
	ExpressionEvaluationController::ExpressionEvaluationController(std::shared_ptr<Parsing::IParser> parser, QObject* parent) :
		m_Parser(std::move(parser)), QObject(parent)
	{

	}

	void ExpressionEvaluationController::TryParseExpression(const QString& expression)
	{
		try
		{
			std::string expressionStr = expression.toStdString();
			auto ast = m_Parser->Parse(expressionStr);
			if (!ast)
			{
				throw std::runtime_error("Parser produced empty AST.");
			}
		}
		catch (const std::runtime_error& error)
		{
			emit ParseErrorOccurred(QString::fromStdString(error.what()), MessageType::Error);
		}
	}

	void ExpressionEvaluationController::EvaluateExpression(const QString& expression, EvaluationMethod method)
	{

	}
}