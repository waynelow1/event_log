#ifndef DISPLAYVIEWDELEGATE_H
#define DISPLAYVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>

class DisplayViewDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    // for Error Red Row Dispaly
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        int severityColumn = 2;
        QString severity = index.model()->data(index.model()->index(index.row(), severityColumn)).toString();

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // If the row contains "ERROR", modify the palette
        if (severity.compare("ERROR", Qt::CaseInsensitive) == 0)
        {
            opt.backgroundBrush = QBrush(Qt::red);
            // Optional: make text white for better contrast
            opt.palette.setBrush(QPalette::Text, Qt::white);
            opt.palette.setBrush(QPalette::HighlightedText, Qt::white);
        }

        // Draw the item with our modified options (used for any theme like Fusion)
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
    }

    // Center Column Text
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);

        int col = index.column();
        if (col == 0 || col == 1 || col == 2 || col == 3)   // ID:0 Timestamp:1 Severity:2, Source:3
        {
            option->displayAlignment = Qt::AlignCenter;
        }
    }
};

#endif // DISPLAYVIEWDELEGATE_H
