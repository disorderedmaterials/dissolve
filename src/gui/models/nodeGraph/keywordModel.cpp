// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "keywordModel.h"

KeywordModel::KeywordModel(KeywordStore *source) : source_(source) {}

KeywordModel &KeywordModel::operator=(const KeywordModel &other)
{
    source_ = other.source_;
    return *this;
}

int KeywordModel::rowCount(const QModelIndex &parent) const
{
    if (!source_)
        return 0;
    return source_->nVisibleKeywords();
}

int KeywordModel::columnCount(const QModelIndex &parent) const
{
    if (!source_)
        return 0;
    return 2;
}

QVariant KeywordModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (index.column() == 1)
        return row;

    auto [keyword, type] = source_->at(row);
    return QString::fromStdString(std::string(keyword->name()));

    // return QString::fromStdString(std::string(keyword.first->name()));
}

bool KeywordModel::setData(const QModelIndex &index, const QVariant &value, int role) { return false; }

// Qt::ItemFlags KeywordModel::flags(const QModelIndex &index) const override;

QVariant KeywordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (section)
    {
        case 0:
            return "Name";
        case 1:
            return "Value";
        default:
            return "ERROR";
    }
}
