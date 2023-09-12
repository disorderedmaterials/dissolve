// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/dissolveModelImageProvider.h"
#include "gui/models/dissolveModel.h"
#include "gui/models/moduleLayerModel.h"

DissolveModelImageProvider::DissolveModelImageProvider(DissolveModel *dissolveModel)
    : QQuickImageProvider(QQmlImageProviderBase::Image)
{
    dissolveModel_ = dissolveModel;
}

QImage DissolveModelImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (id.startsWith(QString("moduleLayer/")))
    {
        auto parts = id.split(QString("/"));
        if (parts.size() != 3)
            return {};
        auto moduleLayerRow = parts[1].toInt();
        auto moduleRow = parts[2].toInt();
        auto moduleLayerIndex = dissolveModel_->moduleLayersModel()->index(moduleLayerRow, 0);
        if (!moduleLayerIndex.isValid())
            return {};
        auto moduleLayerModel =
            dissolveModel_->moduleLayersModel()->data(moduleLayerIndex, Qt::UserRole + 1).value<ModuleLayerModel *>();
        auto moduleIndex = moduleLayerModel->index(moduleRow, 0);
        if (!moduleIndex.isValid())
            return {};
        *size = requestedSize;
        return moduleLayerModel->data(moduleIndex, Qt::DecorationRole).value<QIcon>().pixmap(QSize(16, 16)).toImage();
        ;
    }
    return {};
}