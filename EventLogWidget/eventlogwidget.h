#ifndef EVENTLOGWIDGET_H
#define EVENTLOGWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QFileDialog>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSortFilterProxyModel>

#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QTimer>
#include <QMessageBox>

#include "DisplayViewDelegate.h"
#include "EventFilterProxy.h"


namespace Ui {
class EventLogWidget;
}


class EventLogWidget : public QWidget
{
    Q_OBJECT

public:
    enum class EVENT_TYPE {
        Debug,
        Info,
        Warning,
        Error,
        Critital
    };
    Q_ENUM(EVENT_TYPE)

    explicit EventLogWidget(QWidget *parent = nullptr);
    ~EventLogWidget();

public slots:
    void addEvent(const QDateTime &timestamp, const EVENT_TYPE &eventType, const QString &source, const QString &message);
    void removeEventItem(int index);
    bool exportCSV(const QString &filePath);
    void clearEventLog();

private slots:
    void on_clearEventLogBtn_clicked();
    void on_removeLogItemBtn_clicked();
    void on_exportCSVBtn_clicked();

private:
    QString eventTypeToString(EVENT_TYPE eventType) const;

private:
    Ui::EventLogWidget *ui;

    // database-based table model
    QSqlTableModel *m_sqlTableModel;

    // filter proxy
    QSortFilterProxyModel *m_filterProxy;

    // error display delegate
    DisplayViewDelegate *delegate {nullptr};
};



#endif // EVENTLOGWIDGET_H
