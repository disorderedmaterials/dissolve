// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <QDialog>

#include <QAbstractTableModel>
#include <QModelIndex>

class XmlFFModel : public QAbstractTableModel
{
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
	Q_UNUSED(parent);
	return 1;
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
	Q_UNUSED(parent);
	return 3;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override { return QVariant(QString("Hello")); }
};
