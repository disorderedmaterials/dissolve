#include "sortfilterproxymodel.h"
#include <QAbstractItemModel>
#include <QRegularExpression>

QObject *SortFilterProxyModel::source() const { return sourceModel(); }

void SortFilterProxyModel::setSource(QObject *source) { setSourceModel(qobject_cast<QAbstractItemModel *>(source)); }

QString SortFilterProxyModel::filterString() const { return filterRegularExpression().pattern(); }

void SortFilterProxyModel::setFilterString(QString filter) { setFilterRegularExpression(QRegularExpression(filter)); }
