#ifndef EVENTFILTERPROXY_H
#define EVENTFILTERPROXY_H

#include <QSortFilterProxyModel>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDateTime>
#include "utils.h"


class EventFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit EventFilterProxy(QObject* parent=nullptr)
        : QSortFilterProxyModel(parent) {}

    bool exportCSVFiltered(const QString &filePath) const;
    void setSeverityFilter(const QString& severity);
    void setSourceFilter(const QString& source);
    void setMessageFilter(const QString& message);
    void setFromTimestamp(const QDateTime& from);
    void setToTimestamp(const QDateTime& to);
    bool hasFilter() const;
    bool hasDateFilter() const;
    void setFilterEnabled(bool enabled);
    bool filterEnabled() const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_sevFilter;
    QString m_srcFilter;
    QString m_msgFilter;

    QDateTime m_fromTs;
    QDateTime m_toTs;

    bool m_filterEnabled = false;
};


#endif // EVENTFILTERPROXY_H
