#include "eventlogmodel.h"

EventLogModel::EventLogModel(QObject *parent)
    : QObject{parent}
{
    // Initialize DB connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("eventlog.db");
    if (!db.open())
    {
        qDebug() << "DB open failed:" << db.lastError();
    }

    // Create table
    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS events ("
           "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
           "Timestamp TEXT,"
           "Severity TEXT,"
           "Source TEXT,"
           "Message TEXT"
           ")");

    // Model setup
    m_sqlTableModel = new QSqlTableModel(this, db);
    m_sqlTableModel->setTable("events");
    m_sqlTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_sqlTableModel->select();
}

bool EventLogModel::addEvent(const QDateTime &timestamp,
                             const QString &severity,
                             const QString &source,
                             const QString &message)
{
    QSqlRecord record = m_sqlTableModel->record();
    record.setValue("Timestamp", timestamp.toString("yyyy-MM-dd HH:mm:ss"));
    record.setValue("Severity", severity);
    record.setValue("Source", source);
    record.setValue("Message", message);

    if (!m_sqlTableModel->insertRecord(-1, record))
    {
        qDebug() << "Insert failed:" << m_sqlTableModel->lastError();
        return false;
    }

    return m_sqlTableModel->submitAll();
}

void EventLogModel::removeEvent(int index)
{
    m_sqlTableModel->removeRow(index);
    m_sqlTableModel->submitAll();
}

void EventLogModel::clearAll()
{
    QSqlQuery q("DELETE FROM events");
    m_sqlTableModel->select();
}

bool EventLogModel::exportCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);
    stream << "Timestamp,Severity,Source,Message\n";

    QStringList lines;
    lines.reserve(100000);

    QSqlQuery query("SELECT Timestamp, Severity, Source, Message "
                    "FROM events ORDER BY ID DESC LIMIT 100000");
    if (!query.isActive())
    {
        qDebug() << "Query failed:" << query.lastError();
        return false;
    }

    while (query.next())
    {
        QString ts  = query.value(0).toString().replace(',', ' ');
        QString sev = query.value(1).toString().replace(',', ' ');
        QString src = query.value(2).toString().replace(',', ' ');
        QString msg = query.value(3).toString().replace(',', ' ');

        lines << QString("%1,%2,%3,%4").arg(ts).arg(sev).arg(src).arg(msg);
    }

    stream << lines.join("\n") << "\n";
    return true;
}
