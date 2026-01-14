#ifndef EVENTSIMULATOR_H
#define EVENTSIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QRandomGenerator>
#include "eventlogwidget.h"

class EventSimulator : public QObject
{
    Q_OBJECT
public:
    explicit EventSimulator(QObject *parent = nullptr);

public slots:
    void start();
    void stop();
    void setInterval(int interval);

signals:
    void newEvent(const QDateTime timestamp,
                  const EventLogWidget::EVENT_TYPE &type,
                  const QString &source,
                  const QString &message);

private slots:
    void generateEvent();

private:
    QTimer *m_timer {nullptr};
};

#endif // EVENTSIMULATOR_H
