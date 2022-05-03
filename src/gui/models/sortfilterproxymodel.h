#pragma once

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString)
    Q_PROPERTY(QObject *source READ source WRITE setSource)

    public:
    QObject *source() const;
    void setSource(QObject *source);

    QString filterString() const;
    void setFilterString(QString filter);
};
