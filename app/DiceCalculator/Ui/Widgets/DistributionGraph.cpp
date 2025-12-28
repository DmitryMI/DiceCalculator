#pragma once

#include "DiceCalculator/Ui/Widgets/DistributionGraph.h"


namespace DiceCalculator::Ui::Widgets
{
	DistributionGraph::DistributionGraph(QWidget* parent)
		: QWidget(parent)
	{
       
	}

	void DistributionGraph::SetDistribution(const Distribution& dist)
	{
		m_Distribution = dist;
	}


}