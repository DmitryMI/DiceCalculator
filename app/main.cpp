#include <QApplication>
#include <QWidget>
#include <QCommandLineParser>
#include "DiceCalculator/Ui/MainWindow.h"
#include "DiceCalculator/Logging/LogManager.h"
#include "spdlog/spdlog.h"
#include <QStandardPaths>
#include "DiceCalculator/Parsing/BoostSpiritParser.h"
#include "DiceCalculator/Operators/Registry.h"

using namespace DiceCalculator;

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QApplication::setApplicationName("DiceCalculator");
	QCoreApplication::setApplicationVersion("1.0");

	QCommandLineParser parser;
	parser.setApplicationDescription("DiceCalculator Help");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption logLevelOption(
		"log-level",
		"Set spdlog log level (options: trace, debug, info, warn, error, critical, off)",
		"level",
		"info"
	);
	parser.addOption(logLevelOption);

	QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	QCommandLineOption assetsRootOption(
		"assets-root",
		"Set path to assets root directory",
		"assets-root",
		appDataPath + "/assets"
	);
	parser.addOption(assetsRootOption);

	QCommandLineOption sceneOption(
		"scene",
		"Load scene from this path after startup",
		"scene"
	);
	parser.addOption(sceneOption);

	// Process the command line arguments
	parser.process(app);

	QString levelStr = parser.value(logLevelOption).toLower();
	DiceCalculator::Logging::LogManager::InitLogging(levelStr.toStdString());

	QString assetsRoot = parser.value(assetsRootOption);
	QString sceneRoot = parser.value(sceneOption);

	spdlog::info("DiceCalculator started");
	
	DiceCalculator::Ui::MainWindow mainWindow(std::make_shared<Parsing::BoostSpiritParser>(std::make_shared<Operators::Registry>()));
	mainWindow.show();

	return app.exec();
}