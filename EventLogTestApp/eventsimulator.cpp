#include "eventsimulator.h"

EventSimulator::EventSimulator(QObject *parent) : QObject(parent)
{
}

void EventSimulator::start(int interval)
{
    if (!m_timer)
    {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &EventSimulator::generateEvent);
    }

    m_timer->start(interval);
}

void EventSimulator::stop()
{
    m_timer->stop();
}

void EventSimulator::generateEvent()
{
    // Pick random type
    QList<EventLogWidget::EVENT_TYPE> types = {
        EventLogWidget::EVENT_TYPE::Info,
        EventLogWidget::EVENT_TYPE::Warning,
        EventLogWidget::EVENT_TYPE::Error
    };
    auto type = types.at(QRandomGenerator::global()->bounded(types.size()));

    // Random source
    QStringList sources = {"Sensor", "Camera", "Motor", "PLC", "System"};
    QString source = sources.at(QRandomGenerator::global()->bounded(sources.size()));

    // Random message
    QStringList messages = {
        "Startup OK",
        "Temperature high",
        "Link lost",
        "Overcurrent detected",
        "Reconnected",
        "Heartbeat",
        "Timeout waiting reply"
    };
    QString message = messages.at(QRandomGenerator::global()->bounded(messages.size()));

    emit newEvent(QDateTime::currentDateTime(), type, source, message);
}
