#include "DiceCalculator/Ui/MainWindow.h"
#include "DiceCalculator/Logging/LogManager.h"

namespace DiceCalculator::Ui
{

	MainWindow::MainWindow(QWidget* parent) :
		QMainWindow(parent)
	{
		m_Ui.setupUi(this);

		m_Logger = DiceCalculator::Logging::LogManager::CreateLogger("MainWindow");
		
		m_Logger->info("MainWindow created");
	}

	MainWindow::~MainWindow()
	{
		
	}
}
