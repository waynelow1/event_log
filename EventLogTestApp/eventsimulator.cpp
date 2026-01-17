#include "eventsimulator.h"

EventSimulator::EventSimulator(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &EventSimulator::generateEvent);
}

void EventSimulator::start()
{
    m_timer->start();
}

void EventSimulator::stop()
{
    m_timer->stop();
}

void EventSimulator::setInterval(int interval)
{
    m_timer->setInterval(interval);
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

    // Random message with commas
    QStringList messages = {
        "Startup OK, system ready",
        "Temperature high, value exceeded limit",
        "Link lost, retrying connection",
        "Overcurrent detected, motor stopped",
        "Reconnected, normal operation resumed",
        "Heartbeat received, all good",
        "Timeout waiting reply, PLC not responding"
    };
    QString message = messages.at(QRandomGenerator::global()->bounded(messages.size()));

    emit newEvent(QDateTime::currentDateTime(), type, source, message);
}
