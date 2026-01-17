#ifndef EVENTLOGCONTROLLER_H
#define EVENTLOGCONTROLLER_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QMutex>

#include "eventlogmodel.h"
#include "EventFilterProxy.h"

class EventLogController : public QObject
{
    Q_OBJECT
public:
    explicit EventLogController(QObject* parent = nullptr);
    ~EventLogController();

    void initializeDatabase(const QString& databasePath, const QString& connectionName);

    QAbstractItemModel* model() const;
    bool hasFilter() const;

    void setFlushIntervalMs(int newFlushIntervalMs);
    void setMaxBatchSize(int newMaxBatchSize);

public slots:
    void setSeverityFilter(const QString& s);
    void setSourceFilter(const QString& s);
    void setMessageFilter(const QString& s);
    void setFromTimestamp(const QDateTime& dt);
    void setToTimestamp(const QDateTime& dt);

    bool addEvent(const QDateTime& ts,
                  const QString& sev,
                  const QString& src,
                  const QString& msg);
    void removeEvent(int row);
    void clearAll();
    bool exportCSV(const QString& filePath);

signals:
    void rowsAdded();

private slots:
    void flushQueuedEvents();

private:
    EventLogModel* m_model {nullptr};
    EventFilterProxy* m_proxy {nullptr};

    QTimer* m_flushTimer {nullptr};
    QMutex m_mutex;
    QVector<EventLogEntry> m_queue;

    int m_flushIntervalMs {100};
    int m_maxBatchSize {300};
};

#endif // EVENTLOGCONTROLLER_H
