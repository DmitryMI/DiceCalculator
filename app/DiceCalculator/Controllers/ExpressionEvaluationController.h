#pragma once

#include <QObject>
#include "DiceCalculator/Distribution.h"
#include "DiceCalculator/Parsing/IParser.h"

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
		void EvaluationStarted(const QString& expression);
		void ParseErrorOccurred(const QString& errorMessage, MessageType messageType);
		void EvaluationErrorOccurred(const QString& errorMessage, MessageType messageType);
		void EvaluationCompleted(const QString& message, MessageType messageType);
		void PlotDataReady(const QString& expression, const Distribution& distribution);
		void BusyStateChanged(bool isBusy);

	private:
		bool m_Busy = false;

		std::shared_ptr<Parsing::IParser> m_Parser;
	};
}