#pragma once

#include <QWidget>
#include "DiceCalculator/Distribution.h"
#include "qcustomplot.h"

namespace DiceCalculator::Ui::Widgets
{
    class DistributionGraph : public QWidget
    {
        Q_OBJECT

    public:
        DistributionGraph(QWidget* parent = nullptr);

        ~DistributionGraph() = default;
        
		void SetDistribution(const Distribution& dist);
        void SetExpression(const QString& expression);
        void Plot();
    private:

        Distribution m_Distribution;
		QString m_Expression;
		QCustomPlot* m_Plot = nullptr;
    };
}
