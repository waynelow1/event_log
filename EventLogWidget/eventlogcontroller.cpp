#include "eventlogcontroller.h"

EventLogController::EventLogController(QObject* parent)
    : QObject(parent),
    m_proxy(new EventFilterProxy(this)),
    m_flushTimer(new QTimer(this))
{
    connect(m_flushTimer, &QTimer::timeout, this, &EventLogController::flushQueuedEvents);
}

EventLogController::~EventLogController()
{
    flushQueuedEvents();
}

void EventLogController::initializeDatabase(const QString& dbPath, const QString& connectionName)
{
    if (m_model)
        return;

    m_model = new EventLogModel(dbPath, connectionName, this);
    m_proxy->setSourceModel(m_model->model());
    m_proxy->setDynamicSortFilter(false);
    m_flushTimer->start(m_flushIntervalMs);  // flush events to DB
}

QAbstractItemModel* EventLogController::model() const
{
    return m_proxy;
}

bool EventLogController::addEvent(const QDateTime& ts,
                                  const QString& sev,
                                  const QString& src,
                                  const QString& msg)
{
    QMutexLocker lock(&m_mutex);
    m_queue.push_back({ ts, sev, src, msg });

    if (m_queue.size() >= m_maxBatchSize)
    {
        flushQueuedEvents();
    }

    return true;
}

void EventLogController::flushQueuedEvents()
{
    QVector<EventLogEntry> batch;

    {
        QMutexLocker lock(&m_mutex);
        if (m_queue.isEmpty())
            return;
        batch.swap(m_queue);
    }

    if (m_model->insertBatch(batch))
    {
        m_model->model()->select();
        // while (m_model->model()->canFetchMore())
        // {
        //     m_model->model()->fetchMore();
        // }
        m_proxy->invalidate();
        emit rowsAdded();
    }
}

void EventLogController::setFilterEnabled(bool enabled)
{
    m_proxy->setFilterEnabled(enabled);
}

void EventLogController::setMaxBatchSize(int newMaxBatchSize)
{
    m_maxBatchSize = newMaxBatchSize;
}

void EventLogController::setMaxVisibleRows(int rows)
{
    if (!m_model)
        return;

    flushQueuedEvents();        // ensure DB is up-to-date
    m_model->setMaxVisibleEvents(rows);
    m_proxy->invalidate();
}

void EventLogController::setFlushIntervalMs(int newFlushIntervalMs)
{
    m_flushIntervalMs = newFlushIntervalMs;
}

void EventLogController::removeEvent(int row)
{
    flushQueuedEvents();
    m_model->removeEvent(row);
}

void EventLogController::clearAll()
{
    flushQueuedEvents();
    m_model->clearAll();
}

bool EventLogController::exportCSV(const QString& path)
{
    flushQueuedEvents();
    return hasFilter() ? m_proxy->exportCSVFiltered(path) : m_model->exportCSV(path);
}

bool EventLogController::hasFilter() const
{
    return m_proxy->hasFilter() || m_proxy->hasDateFilter();
}

void EventLogController::setSeverityFilter(const QString& s)
{
    m_proxy->setSeverityFilter(s);
}

void EventLogController::setSourceFilter(const QString& s)
{
    m_proxy->setSourceFilter(s);
}

void EventLogController::setMessageFilter(const QString& s)
{
    m_proxy->setMessageFilter(s);
}

void EventLogController::setFromTimestamp(const QDateTime& d)
{
    m_proxy->setFromTimestamp(d);
}

void EventLogController::setToTimestamp(const QDateTime& d)
{
    m_proxy->setToTimestamp(d);
}
