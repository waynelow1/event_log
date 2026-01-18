#ifndef EVENTLOGMODEL_H
#define EVENTLOGMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QDateTime>
#include <QVector>
#include <QFile>
#include "utils.h"

struct EventLogEntry
{
    QDateTime timestamp;
    QString severity;
    QString source;
    QString message;
};

class EventLogModel : public QObject
{
    Q_OBJECT
public:
    explicit EventLogModel(const QString& databasePath,
                           const QString& connectionName,
                           QObject* parent = nullptr);

    QSqlTableModel* model() const { return m_sqlTableModel; }

    bool addEvent(const QDateTime& timestamp,
                  const QString& severity,
                  const QString& source,
                  const QString& message);

    bool insertBatch(const QVector<EventLogEntry>& batch);

    void removeEvent(int index);
    void clearAll();
    bool exportCSV(const QString& filePath);

    void setMaxVisibleEvents(int maxEvents);

private:
    QSqlTableModel* m_sqlTableModel {nullptr};
    QString m_connectionName;

    int m_maxVisibleEvents {1000};
};

#endif // EVENTLOGMODEL_H
