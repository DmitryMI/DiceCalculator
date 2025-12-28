
#include <QObject>
#include "DiceCalculator/Controllers/ExpressionEvaluationController.h"
#include "DiceCalculator/Evaluation/ConvolutionAstVisitor.h"
#include "DiceCalculator/Evaluation/CombinationAstVisitor.h"
#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Expressions/DiceAst.h"

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

			emit ParsingMessage(expression, "Parsing OK", MessageType::Info);
			emit ParsingFinished(expression);
		}
		catch (const std::runtime_error& error)
		{
			emit ParsingMessage(expression, QString::fromStdString(error.what()), MessageType::Error);
			emit ParsingFinished(expression);
		}
	}

	void ExpressionEvaluationController::EvaluateExpression(const QString& expression, EvaluationMethod method)
	{
		try
		{
			std::string expressionStr = expression.toStdString();
			auto ast = m_Parser->Parse(expressionStr);
			if (!ast)
			{
				throw std::runtime_error("Parser produced empty AST.");
			}
			emit ParsingMessage(expression, "Parsing OK", MessageType::Info);
			emit ParsingFinished(expression);

			EvaluateExpressionInternalWrapper(ast, method, expression);
		}
		catch (const std::runtime_error& error)
		{
			emit ParsingMessage(expression, QString::fromStdString(error.what()), MessageType::Error);
			emit ParsingFinished(expression);
		}
	}

	void ExpressionEvaluationController::EvaluateExpressionInternalWrapper(std::shared_ptr<DiceCalculator::Expressions::DiceAst> ast, EvaluationMethod method, const QString& originalExpression)
	{
		emit EvaluationStarted(originalExpression);

		if (method == EvaluationMethod::Auto)
		{
			int methodMax = static_cast<int>(EvaluationMethod::Roll);
			for (int methodIndex = static_cast<int>(EvaluationMethod::Convolution); methodIndex <= methodMax; ++methodIndex)
			{
				EvaluationMethod currentMethod = static_cast<EvaluationMethod>(methodIndex);
				try
				{
					EvaluateExpressionInternal(ast, currentMethod, originalExpression);
					emit EvaluationMessage(originalExpression, "Evaluation OK", MessageType::Info);
					return;
				}
				catch (const std::runtime_error& error)
				{
					emit EvaluationMessage(
						originalExpression,
						QString("Evaluation with method %1 failed: %2")
						.arg(static_cast<int>(currentMethod))
						.arg(QString::fromStdString(error.what())),
						MessageType::Warning
					);
					continue;
				}
			}
			emit EvaluationMessage(originalExpression, "All evaluation methods failed.", MessageType::Error);
		}
		else if (method == EvaluationMethod::Convolution)
		{
			try
			{
				EvaluateExpressionInternal(ast, method, originalExpression);
				emit EvaluationMessage(originalExpression, "Evaluation OK", MessageType::Info);

			}
			catch (const std::runtime_error& error)
			{
				emit EvaluationMessage(
					originalExpression,
					QString("Evaluation with Convolution method failed: %1")
					.arg(QString::fromStdString(error.what())),
					MessageType::Error
				);
			}

		}
		else if (method == EvaluationMethod::Combinatorial)
		{
			try
			{
				EvaluateExpressionInternal(ast, method, originalExpression);
			}
			catch (const std::runtime_error& error)
			{
				emit EvaluationMessage(
					originalExpression,
					QString("Evaluation with Combinatorial method failed: %1")
					.arg(QString::fromStdString(error.what())),
					MessageType::Error
				);
			}
			
		}
		else if (method == EvaluationMethod::Roll)
		{
			try
			{
				EvaluateExpressionInternal(ast, method, originalExpression);
				emit EvaluationMessage(originalExpression, "Evaluation OK", MessageType::Info);
			}
			catch (const std::runtime_error& error)
			{
				emit EvaluationMessage(
					originalExpression,
					QString("Evaluation with Roll method failed: %1")
					.arg(QString::fromStdString(error.what())),
					MessageType::Error
				);
			}
		}
		else
		{
			throw std::runtime_error("Unknown evaluation method.");
		}

		emit EvaluationFinished(originalExpression);
	}

	void ExpressionEvaluationController::EvaluateExpressionInternal(std::shared_ptr<DiceCalculator::Expressions::DiceAst> ast, EvaluationMethod method, const QString& originalExpression)
	{
		Distribution dist;
		if (method == EvaluationMethod::Convolution)
		{
			Evaluation::ConvolutionAstVisitor visitor;
			ast->Accept(visitor);
			dist = visitor.GetDistribution();
		}
		else if (method == EvaluationMethod::Combinatorial)
		{
			Evaluation::CombinationAstVisitor visitor;
			ast->Accept(visitor);
			auto combinations = visitor.GetCombinations();
			dist = Distribution::FromCombinations(combinations);
		}
		else if(method == EvaluationMethod::Roll)
		{
			Evaluation::RollAstVisitor visitor(m_Random);
			for (int i = 0; i < MaxRollsForRollMethod; ++i)
			{
				ast->Accept(visitor);
				int result = visitor.GetResult();
				dist.AddOutcome(result, 1.0);
			}
			dist.Normalize();
		}
		else
		{
			throw std::runtime_error("Unknown evaluation method.");
		}

		emit PlotDataReady(originalExpression, dist);
	}
}