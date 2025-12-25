#pragma once

#include <qmainwindow.h>
#include <spdlog/spdlog.h>
#include "ui_MainWindow.h"

namespace DiceCalculator::Ui
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		MainWindow(QWidget* parent = nullptr);
		virtual ~MainWindow();

	private:
		std::shared_ptr<spdlog::logger> m_Logger;

		// Widgets
		Ui_MainWindow m_Ui;
	};
}