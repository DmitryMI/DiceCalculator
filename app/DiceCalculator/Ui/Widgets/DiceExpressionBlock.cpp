#include "DiceCalculator/Ui/Widgets/DiceExpressionBlock.h"
#include "DiceCalculator/Ui/Widgets/DiceExpressionInput.h"
#include "DiceCalculator/Controllers/ExpressionEvaluationController.h"
#include "DiceCalculator/Ui/Widgets/DistributionGraph.h"
#include <QPushButton>

namespace DiceCalculator::Ui::Widgets
{
	DiceExpressionBlock::DiceExpressionBlock(
		DiceCalculator::Controllers::ExpressionEvaluationController* controller,
		QWidget* parent
	): 
		m_Controller(controller),
		QWidget(parent)
	{
		m_Ui.setupUi(this);

		assert(m_Controller);

		m_Controller->setParent(this);

		m_DiceExpressionInput = new DiceExpressionInput(this);
		m_DiceExpressionInput->setSizePolicy(m_Ui.diceExpressionInputPlaceholder->sizePolicy());
		m_DiceExpressionInput->setMinimumSize(m_Ui.diceExpressionInputPlaceholder->minimumSize());
		m_Ui.gridLayout_2->replaceWidget(m_Ui.diceExpressionInputPlaceholder, m_DiceExpressionInput);
		m_Ui.diceExpressionInputPlaceholder->deleteLater();
		m_Ui.diceExpressionInputPlaceholder = nullptr;

		connect(m_Ui.removeBlockButton, &QPushButton::clicked, this, [this]() {
			emit RemoveRequested(this);
		});


		connect(
			m_DiceExpressionInput,
			&DiceExpressionInput::ExpressionEvaluationRequested,
			m_Controller,
			&DiceCalculator::Controllers::ExpressionEvaluationController::EvaluateExpression
		);

		connect(m_DiceExpressionInput, &DiceExpressionInput::ExpressionChanged, this, [this](const QString& expression) {
			m_DiceExpressionInput->ClearMessages();
			m_Controller->TryParseExpression(expression);
			});

		connect(m_Controller, &DiceCalculator::Controllers::ExpressionEvaluationController::EvaluationStarted, this, [this](const QString& expression) {
			m_DiceExpressionInput->ClearMessages();
			});

		connect(m_Controller, &DiceCalculator::Controllers::ExpressionEvaluationController::ParsingFinished, this, [this](const QString& errorMessage, DiceCalculator::Controllers::ExpressionEvaluationController::MessageType messageType) {
			m_DiceExpressionInput->AppendMessage(errorMessage, messageType);
			});

		connect(m_Controller, &DiceCalculator::Controllers::ExpressionEvaluationController::EvaluationMessage, this, [this](const QString& errorMessage, DiceCalculator::Controllers::ExpressionEvaluationController::MessageType messageType) {
			m_DiceExpressionInput->AppendMessage(errorMessage, messageType);
			});

		connect(m_Controller, &DiceCalculator::Controllers::ExpressionEvaluationController::EvaluationFinished, this, [this](const QString& message, DiceCalculator::Controllers::ExpressionEvaluationController::MessageType messageType) {
			m_DiceExpressionInput->AppendMessage(message, messageType);
			});

		connect(m_Controller, &DiceCalculator::Controllers::ExpressionEvaluationController::BusyStateChanged, this, [this](bool isBusy) {
			m_DiceExpressionInput->setEnabled(!isBusy);
			});

		connect(m_Controller, &DiceCalculator::Controllers::ExpressionEvaluationController::PlotDataReady, this,
			[this](const QString& expression, const Distribution& distribution) {ClearOutput(); AddPlot(expression, distribution); });
		
	}

	DiceExpressionBlock::~DiceExpressionBlock()
	{
		m_Controller->setParent(nullptr);
		m_Controller->deleteLater();
		m_Controller = nullptr;
	}

	void DiceExpressionBlock::ClearOutput()
	{
		QLayout* scrollLayout = m_Ui.scrollAreaWidget->layout();
		if (scrollLayout)
		{
			QLayoutItem* item;
			while ((item = scrollLayout->takeAt(0)) != nullptr)
			{
				if (item->widget())
				{
					item->widget()->deleteLater();
				}
				delete item;
			}
		}
	}

	void DiceExpressionBlock::AddPlot(const QString& expression, const Distribution& distribution)
	{
		auto graph = new DistributionGraph(this);
		graph->SetDistribution(distribution);
		graph->SetExpression(expression);
		graph->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QHBoxLayout* scrollLayout = dynamic_cast<QHBoxLayout*>(m_Ui.scrollAreaWidget->layout());
		if (!scrollLayout)
		{
			scrollLayout = new QHBoxLayout(m_Ui.scrollAreaWidget);
			m_Ui.scrollAreaWidget->setLayout(scrollLayout);
		}

		scrollLayout->addWidget(graph);
		graph->Plot();
	}
}