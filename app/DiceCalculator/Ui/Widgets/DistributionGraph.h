#pragma once

#include <QWidget>
#include "DiceCalculator/Distribution.h"

namespace DiceCalculator::Ui::Widgets
{
    class DistributionGraph : public QWidget
    {
        Q_OBJECT

    public:
        DistributionGraph(QWidget* parent = nullptr);

        ~DistributionGraph() = default;
        
		void SetDistribution(const Distribution& dist);
    private:

        Distribution m_Distribution;
    };
}
