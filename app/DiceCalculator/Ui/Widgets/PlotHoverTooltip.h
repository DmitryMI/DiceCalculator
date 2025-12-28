#pragma once

class PlotHoverTooltip : public QLabel
{
public:
    explicit PlotHoverTooltip(QWidget* parent = nullptr)
        : QLabel(parent)
    {
        setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_ShowWithoutActivating);
        setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        setStyleSheet(
            "QLabel {"
            "  background-color: #ffffe1;"
            "  border: 1px solid gray;"
            "  padding: 4px;"
            "  font-size: 9pt;"
            "}"
        );

        hide();
    }
};
