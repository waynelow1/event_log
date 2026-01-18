#include "EventFilterProxy.h"


bool EventFilterProxy::exportCSVFiltered(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);

    stream << "Timestamp,Severity,Source,Message\n";

    for (int r = 0; r < rowCount(); ++r)
    {
        QModelIndex proxyIndex = index(r, 0);
        QModelIndex srcIndex = mapToSource(proxyIndex);

        QSqlTableModel* sqlModel = qobject_cast<QSqlTableModel*>(sourceModel());
        if (!sqlModel)
            return false;

        QSqlRecord rec = sqlModel->record(srcIndex.row());

        QString ts  = csvEscape(rec.value("Timestamp").toString());
        QString sev = csvEscape(rec.value("Severity").toString());
        QString src = csvEscape(rec.value("Source").toString());
        QString msg = csvEscape(rec.value("Message").toString());

        stream << ts << "," << sev << "," << src << "," << msg << "\n";
    }

    return true;
}

// Setters
void EventFilterProxy::setSeverityFilter(const QString& severity)
{
    if (m_sevFilter == severity)
        return;
    m_sevFilter = severity;
    invalidateFilter();
}

void EventFilterProxy::setSourceFilter(const QString& source)
{
    if (m_srcFilter == source)
        return;
    m_srcFilter = source;
    invalidateFilter();
}

void EventFilterProxy::setMessageFilter(const QString& message)
{
    if (m_msgFilter == message)
        return;
    m_msgFilter = message;
    invalidateFilter();
}

void EventFilterProxy::setFromTimestamp(const QDateTime& from)
{
    if (m_fromTs == from)
        return;
    m_fromTs = from;
    invalidateFilter();
}

void EventFilterProxy::setToTimestamp(const QDateTime& to)
{
    if (m_toTs == to)
        return;
    m_toTs = to;
    invalidateFilter();
}

bool EventFilterProxy::hasFilter() const
{
    if (!m_filterEnabled)
        return false;

    return ((m_sevFilter != "All" && !m_sevFilter.isEmpty()) ||
           !m_srcFilter.isEmpty() ||
           !m_msgFilter.isEmpty());
}

bool EventFilterProxy::hasDateFilter() const
{
    if (!m_filterEnabled)
        return false;

    return (m_fromTs.isValid() || m_toTs.isValid());
}

bool EventFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (!m_filterEnabled)
        return true;

    auto model = sourceModel();

    QString tsStr = model->index(sourceRow, 1, sourceParent).data().toString();
    QDateTime rowTs = QDateTime::fromString(tsStr, "yyyy-MM-dd HH:mm:ss");

    // FROM filter
    if (m_fromTs.isValid() && rowTs < m_fromTs)
        return false;

    // TO filter
    if (m_toTs.isValid() && rowTs > m_toTs)
        return false;

    QString sev = model->index(sourceRow, 2, sourceParent).data().toString(); // Severity
    QString src = model->index(sourceRow, 3, sourceParent).data().toString(); // Source
    QString msg = model->index(sourceRow, 4, sourceParent).data().toString(); // Message

    // Severity filter (exact match unless "All")
    if (!m_sevFilter.isEmpty() && m_sevFilter != "All" && sev != m_sevFilter)
        return false;

    // Source substring match
    if (!m_srcFilter.isEmpty() && !src.contains(m_srcFilter, Qt::CaseInsensitive))
        return false;

    // Message substring match
    if (!m_msgFilter.isEmpty() && !msg.contains(m_msgFilter, Qt::CaseInsensitive))
        return false;

    return true;
}

bool EventFilterProxy::filterEnabled() const
{
    return m_filterEnabled;
}

void EventFilterProxy::setFilterEnabled(bool enabled)
{
    if (m_filterEnabled == enabled)
        return;

    m_filterEnabled = enabled;
    invalidateFilter();
}
