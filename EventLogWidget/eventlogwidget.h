#ifndef EVENTLOGWIDGET_H
#define EVENTLOGWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>

#include "eventlogcontroller.h"
#include "DisplayViewDelegate.h"


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
        Critical
    };
    Q_ENUM(EVENT_TYPE)

    explicit EventLogWidget(QWidget *parent = nullptr);
    ~EventLogWidget();

    void initializeModel(const QString &databasePath, const QString connectionName);
    void setFlushIntervalMs(int newFlushIntervalMs);
    void setMaxBatchSize(int newMaxBatchSize);

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
    Ui::EventLogWidget *ui;

    QString eventTypeToString(EVENT_TYPE eventType) const;

    EventLogController* m_controller {nullptr};

    // error display delegate
    DisplayViewDelegate *delegate {nullptr};
};



#endif // EVENTLOGWIDGET_H
