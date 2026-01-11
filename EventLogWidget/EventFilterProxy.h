#ifndef EVENTFILTERPROXY_H
#define EVENTFILTERPROXY_H

#include <QSortFilterProxyModel>


class EventFilterProxy : public QSortFilterProxyModel
{
public:
    EventFilterProxy(QObject* parent=nullptr)
        : QSortFilterProxyModel(parent) {}

    QString sevFilter;
    QString srcFilter;
    QString msgFilter;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        auto model = sourceModel();

        QString sev = model->index(sourceRow, 2, sourceParent).data().toString(); // Severity
        QString src = model->index(sourceRow, 3, sourceParent).data().toString(); // Source
        QString msg = model->index(sourceRow, 4, sourceParent).data().toString(); // Message

        // Severity filter (exact match unless "All")
        if (!sevFilter.isEmpty() && sevFilter != "All" && sev != sevFilter)
            return false;

        // Source substring match
        if (!srcFilter.isEmpty() && !src.contains(srcFilter, Qt::CaseInsensitive))
            return false;

        // Message substring match
        if (!msgFilter.isEmpty() && !msg.contains(msgFilter, Qt::CaseInsensitive))
            return false;

        return true;
    }
};


#endif // EVENTFILTERPROXY_H
