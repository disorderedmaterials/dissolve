// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/dissolveModelImageProvider.h"

DissolveModelImageProvider::DissolveModelImageProvider(DissolveModel& dissolveModel)  : QQuickImageProvider(QQmlImageProviderBase::Image) {dissolveModel_ = dissolveModel;}

QPixmap requestPixmap(const QString &id, QSize *, const QSize &) override;
{
    if (id.startsWith(QString("moduleLayer/")))
    {
        auto idx = id.split(QString("moduleLayer/")).back().toInt();
        return dissolveModel_->moduleLayersModel()->data(dissolveModel_->moduleLayersModel()->index(idx, 0), Qt::DecorationRole).value<QIcon>().pixmap(QSize(16,16));
    }
    return {};
}