#pragma once

#include "DiceCalculator/Ui/Widgets/DistributionGraph.h"


namespace DiceCalculator::Ui::Widgets
{
	DistributionGraph::DistributionGraph(QWidget* parent)
		: QWidget(parent)
	{
		m_Plot = new QCustomPlot(this);
		
		QLayout* layout = new QVBoxLayout(this);
		layout->addWidget(m_Plot);
		setLayout(layout);
	}

	void DistributionGraph::SetDistribution(const Distribution& dist)
	{
		m_Distribution = dist;
	}

	void DistributionGraph::SetExpression(const QString& expression)
	{
		m_Expression = expression;
	}

	void DistributionGraph::Plot()
	{
		QVector<double> x, y;

		for (const auto& [value, probability] : m_Distribution)
		{
			x << static_cast<double>(value);
			y << probability;
		}

		QCPBars* bars = new QCPBars(m_Plot->xAxis, m_Plot->yAxis);
		bars->setData(x, y);

		m_Plot->xAxis->setLabel("Result");
		m_Plot->yAxis->setLabel("Probability");
		// m_Plot->yAxis->setRange(0, 0.1);

		m_Plot->rescaleAxes();
		m_Plot->replot();
	}


}