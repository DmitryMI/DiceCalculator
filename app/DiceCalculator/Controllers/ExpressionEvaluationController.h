#pragma once

#include <QObject>
#include "DiceCalculator/Distribution.h"
#include "DiceCalculator/Parsing/IParser.h"
#include "DiceCalculator/StdRandom.h"

namespace DiceCalculator::Controllers
{
	class ExpressionEvaluationController : public QObject
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

		ExpressionEvaluationController(std::shared_ptr<Parsing::IParser> parser, QObject* parent = nullptr);
		~ExpressionEvaluationController() = default;
		
		void TryParseExpression(const QString& expression);
		void EvaluateExpression(const QString& expression, EvaluationMethod method);

	signals:
		void ParsingStarted(const QString& expression);
		void ParsingMessage(const QString& expression, const QString& message, MessageType messageType);
		void ParsingFinished(const QString& expression);

		void EvaluationStarted(const QString& expression);
		void EvaluationMessage(const QString& expression, const QString& message, MessageType messageType);
		void EvaluationFinished(const QString& expression);

		void PlotDataReady(const QString& expression, const Distribution& distribution);
		void BusyStateChanged(bool isBusy);

	private:
		constexpr static int MaxRollsForRollMethod = 10000;

		bool m_Busy = false;
		StdRandom m_Random;

		std::shared_ptr<Parsing::IParser> m_Parser;
		void EvaluateExpressionInternalWrapper(std::shared_ptr<DiceCalculator::Expressions::DiceAst> ast, EvaluationMethod method, const QString& originalExpression);
		void EvaluateExpressionInternal(std::shared_ptr<DiceCalculator::Expressions::DiceAst> ast, EvaluationMethod method, const QString& originalExpression);
	};
}