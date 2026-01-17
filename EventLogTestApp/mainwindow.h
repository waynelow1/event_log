#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QStandardPaths>
#include "eventlogwidget.h"
#include "eventsimulator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionShow_EventLog_Widget_triggered();
    void on_addEventLogBtn_clicked();
    void on_startThreadBtn_clicked();
    void on_stopThreadBtn_clicked();

signals:
    void addEvent(const QDateTime &timestamp, const EventLogWidget::EVENT_TYPE &eventType, const QString &source, const QString &message);

private:
    Ui::MainWindow *ui;

    // thread event
    EventSimulator *worker {nullptr};
    QThread *thread {nullptr};
};
#endif // MAINWINDOW_H
