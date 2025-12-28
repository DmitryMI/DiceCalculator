#pragma once

#include <qmainwindow.h>
#include <spdlog/spdlog.h>
#include <QSpacerItem>
#include <memory>
#include "DiceCalculator/Ui/Widgets/DiceExpressionBlock.h"
#include "ui_MainWindow.h"
#include "DiceCalculator/Parsing/IParser.h"

namespace DiceCalculator::Controllers
{
	class ApplicationController;
}

namespace DiceCalculator::Ui
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		MainWindow(std::shared_ptr<Parsing::IParser> parser, QWidget* parent = nullptr);
		virtual ~MainWindow();

	private:
		std::shared_ptr<spdlog::logger> m_Logger;

		// Widgets
		Ui_MainWindow m_Ui;
		QSpacerItem* m_ScrollAreaSpacerItem{ nullptr };
		std::shared_ptr<Parsing::IParser> m_Parser;

		void AddExpressionBlock();
		void RemoveExpressionBlock(Widgets::DiceExpressionBlock* block);
	};
}