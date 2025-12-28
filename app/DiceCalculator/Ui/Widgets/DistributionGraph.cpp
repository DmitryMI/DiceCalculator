#pragma once

#include "DiceCalculator/Ui/Widgets/DistributionGraph.h"


namespace DiceCalculator::Ui::Widgets
{
	DistributionGraph::DistributionGraph(QWidget* parent)
		: QWidget(parent)
	{
		m_Plot = new QCustomPlot(this);
		auto ticker = QSharedPointer<QCPAxisTickerFixed>::create();
		ticker->setTickStep(1.0);
		ticker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
		ticker->setTickOrigin(0.0);
		m_Plot->xAxis->setTicker(ticker);
		m_Title = new QCPTextElement(
			m_Plot,
			"d20 Probability Mass Function",
			QFont("sans", 12)
		);
		m_Plot->plotLayout()->insertRow(0);
		m_Plot->plotLayout()->addElement(0, 0, m_Title);

		m_Plot->setMouseTracking(true);
		connect(m_Plot, &QCustomPlot::mouseMove,
			this, &DistributionGraph::OnMouseMoveInPlotArea);

		m_Bars = new QCPBars(m_Plot->xAxis, m_Plot->yAxis);

		m_Tooltip = new PlotHoverTooltip(m_Plot);
		
		QLayout* layout = new QVBoxLayout(this);
		layout->addWidget(m_Plot);
		setLayout(layout);

		setMinimumWidth(PlotMinimumWidth);
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
		assert(m_Distribution.Size() > 0);

		QVector<double> x, y;

		for (const auto& [value, probability] : m_Distribution)
		{
			x << static_cast<double>(value);
			y << probability;
		}

		int width = std::max(PlotMinimumWidth, (int)(x.size() * 20));
		setMinimumWidth(width);

		m_Bars->setData(x, y);

		m_Plot->xAxis->setLabel("Result");
		m_Plot->yAxis->setLabel("Probability");
		// m_Plot->xAxis->setRange(x.first() - 10, x.last() + 10);
		m_Title->setText(m_Expression);

		m_Plot->rescaleAxes();
		m_Plot->replot();
	}

	void DistributionGraph::OnMouseMoveInPlotArea(QMouseEvent* event)
	{
		double x = m_Plot->xAxis->pixelToCoord(event->pos().x());
		double y = m_Plot->yAxis->pixelToCoord(event->pos().y());

		// Round to nearest integer bar center
		int value = static_cast<int>(std::round(x));

		auto minMax = m_Distribution.GetMinMax();
		// Check range
		if (value < minMax.first || value > minMax.second)
		{
			m_Tooltip->hide();
			return;
		}

		// Find bar index
		int index = value - minMax.first;

		if (index < 0 || index >= m_Bars->dataCount())
		{
			m_Tooltip->hide();
			return;
		}

		double probability = m_Bars->data()->at(index)->value;

		// Optional: check Y is actually inside the bar
		if (y < 0 || y > probability)
		{
			m_Tooltip->hide();
			return;
		}

		m_Tooltip->setText(
			QString("Roll: %1\nP: %2")
			.arg(value)
			.arg(probability, 0, 'f', 2)
		);

		// Follow cursor (offset so it doesn't obscure the bar)
		QPoint pos = event->globalPosition().toPoint() + QPoint(20, 20);
		m_Tooltip->move(pos);
		m_Tooltip->show();
	}


}