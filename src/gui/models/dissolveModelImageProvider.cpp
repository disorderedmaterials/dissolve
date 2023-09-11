// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/dissolveModelImageProvider.h"
#include "gui/models/dissolveModel.h"

DissolveModelImageProvider::DissolveModelImageProvider(DissolveModel* dissolveModel)  : QQuickImageProvider(QQmlImageProviderBase::Image) {dissolveModel_ = dissolveModel;}

QPixmap DissolveModelImageProvider::requestPixmap(const QString &id, QSize *, const QSize &)
{
    if (id.startsWith(QString("moduleLayer/")))
    {
        auto idx = id.split(QString("moduleLayer/")).back().toInt();
        auto index = dissolveModel_->moduleLayersModel()->index(idx, 0);
        if (!index.isValid())
            return {};
        return dissolveModel_->moduleLayersModel()->data(index, Qt::DecorationRole).value<QIcon>().pixmap(QSize(16,16));
    }
    return {};
}