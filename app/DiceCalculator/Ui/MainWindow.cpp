#include "DiceCalculator/Ui/MainWindow.h"
#include "DiceCalculator/Logging/LogManager.h"
#include "DiceCalculator/Ui/Widgets/DiceExpressionBlock.h"
#include "DiceCalculator/Controllers/ExpressionEvaluationController.h"
#include <qboxlayout.h>

namespace DiceCalculator::Ui
{

	MainWindow::MainWindow(std::shared_ptr<Parsing::IParser> parser, QWidget* parent) :
		m_Parser(std::move(parser)),
		QMainWindow(parent)
	{
		m_Logger = DiceCalculator::Logging::LogManager::CreateLogger("MainWindow");

		m_Ui.setupUi(this);

		m_Ui.scrollArea->setWidgetResizable(true);
		m_Ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

		AddExpressionBlock();

		connect(m_Ui.actionAdd_Expression, &QAction::triggered, this, &MainWindow::AddExpressionBlock);
		
		m_Logger->info("MainWindow created");
	}

	MainWindow::~MainWindow()
	{
		
	}

	void MainWindow::AddExpressionBlock()
	{
		auto* scrollAreaWidgetLayout = m_Ui.scrollAreaWidget->layout();
		if (!scrollAreaWidgetLayout)
		{
			scrollAreaWidgetLayout = new QVBoxLayout();
			m_Ui.scrollAreaWidget->setLayout(scrollAreaWidgetLayout);
		}

		if (!m_ScrollAreaSpacerItem)
		{
			m_ScrollAreaSpacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
		}

		scrollAreaWidgetLayout->removeItem(m_ScrollAreaSpacerItem);

		auto* controller = new Controllers::ExpressionEvaluationController(m_Parser);
		auto* diceExpressionBlock = new Widgets::DiceExpressionBlock(controller, m_Ui.scrollAreaWidgetContents);
		diceExpressionBlock->setSizePolicy(m_Ui.scrollAreaWidget->sizePolicy());
		diceExpressionBlock->setMinimumSize(m_Ui.scrollAreaWidget->minimumSize());

		scrollAreaWidgetLayout->addWidget(diceExpressionBlock);

		scrollAreaWidgetLayout->addItem(m_ScrollAreaSpacerItem);

		connect(diceExpressionBlock, &Widgets::DiceExpressionBlock::RemoveRequested, this, &MainWindow::RemoveExpressionBlock);
	}

	void MainWindow::RemoveExpressionBlock(Widgets::DiceExpressionBlock* block)
	{
		auto* scrollAreaWidgetLayout = m_Ui.scrollAreaWidget->layout();
		if (!scrollAreaWidgetLayout)
		{
			return;
		}
		scrollAreaWidgetLayout->removeWidget(block);
		block->deleteLater();
	}
}
