#ifndef EVENTLOGMODEL_H
#define EVENTLOGMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include <QFile>
#include <QDebug>

class EventLogModel : public QObject
{
    Q_OBJECT

public:
    explicit EventLogModel(QObject *parent = nullptr);

    QSqlTableModel* model() const { return m_sqlTableModel; }

    bool addEvent(const QDateTime &timestamp,
                  const QString &severity,
                  const QString &source,
                  const QString &message);

    void removeEvent(int index);
    void clearAll();
    bool exportCSV(const QString &filePath);

private:
    QSqlTableModel *m_sqlTableModel {nullptr};
};
#endif // EVENTLOGMODEL_H
