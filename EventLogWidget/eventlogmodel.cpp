#include "eventlogmodel.h"

EventLogModel::EventLogModel(const QString& databasePath,
                             const QString& connectionName,
                             QObject* parent)
    : QObject(parent),
    m_connectionName(connectionName)
{
    QSqlDatabase db;

    if (QSqlDatabase::contains(m_connectionName))
    {
        db = QSqlDatabase::database(m_connectionName);
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
        db.setDatabaseName(databasePath);
        db.open();
        QSqlQuery pragmaQuery(db);
        pragmaQuery.exec("PRAGMA journal_mode=WAL;");
        pragmaQuery.exec("PRAGMA synchronous=NORMAL;");
    }

    QSqlQuery q(db);
    q.exec(R"(
        CREATE TABLE IF NOT EXISTS events (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Timestamp TEXT,
            Severity TEXT,
            Source TEXT,
            Message TEXT
        )
    )");

    m_sqlTableModel = new QSqlTableModel(this, db);
    m_sqlTableModel->setTable("events");
    m_sqlTableModel->select();
}

bool EventLogModel::insertBatch(const QVector<EventLogEntry>& batch)
{
    if (batch.isEmpty())
        return true;

    QSqlDatabase db = m_sqlTableModel->database();
    QSqlQuery q(db);

    db.transaction();

    q.prepare(R"(
        INSERT INTO events (Timestamp, Severity, Source, Message)
        VALUES (:ts, :sev, :src, :msg)
    )");

    for (const auto& e : batch)
    {
        q.bindValue(":ts", e.timestamp.toString("yyyy-MM-dd HH:mm:ss"));
        q.bindValue(":sev", e.severity);
        q.bindValue(":src", e.source);
        q.bindValue(":msg", e.message);

        if (!q.exec())
        {
            db.rollback();
            return false;
        }
    }

    db.commit();
    m_sqlTableModel->select();
    return true;
}

bool EventLogModel::addEvent(const QDateTime& ts,
                             const QString& sev,
                             const QString& src,
                             const QString& msg)
{
    return insertBatch({ { ts, sev, src, msg } });
}

void EventLogModel::removeEvent(int index)
{
    QSqlQuery q(m_sqlTableModel->database());
    q.prepare("DELETE FROM events WHERE ID=:id");
    q.bindValue(":id", m_sqlTableModel->record(index).value("ID"));
    q.exec();
    m_sqlTableModel->select();
}

void EventLogModel::clearAll()
{
    QSqlQuery q(m_sqlTableModel->database());
    q.exec("DELETE FROM events");
    q.exec("VACUUM");
    m_sqlTableModel->select();
}

bool EventLogModel::exportCSV(const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream s(&f);
    s << "Timestamp,Severity,Source,Message\n";

    QSqlQuery q(m_sqlTableModel->database());
    q.exec("SELECT Timestamp,Severity,Source,Message FROM events ORDER BY ID DESC");

    while (q.next())
        s << csvEscape(q.value(0).toString()) << ","
          << csvEscape(q.value(1).toString()) << ","
          << csvEscape(q.value(2).toString()) << ","
          << csvEscape(q.value(3).toString()) << "\n";

    return true;
}
