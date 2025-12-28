#pragma once

#include <QWidget>
#include "DiceCalculator/Distribution.h"
#include "qcustomplot.h"
#include "DiceCalculator/Ui/Widgets/PlotHoverTooltip.h"

namespace DiceCalculator::Ui::Widgets
{
    class DistributionGraph : public QWidget
    {
        Q_OBJECT

    public:
        constexpr static int PlotMinimumWidth = 400;

        DistributionGraph(QWidget* parent = nullptr);

        ~DistributionGraph() = default;
        
		void SetDistribution(const Distribution& dist);
        void SetExpression(const QString& expression);
        void Plot();
    private:

        Distribution m_Distribution;
		QString m_Expression;
		QCustomPlot* m_Plot = nullptr;
		QCPTextElement* m_Title = nullptr;
        QCPBars* m_Bars = nullptr;
        PlotHoverTooltip* m_Tooltip = nullptr;

		void OnMouseMoveInPlotArea(QMouseEvent* event);
    };
}
