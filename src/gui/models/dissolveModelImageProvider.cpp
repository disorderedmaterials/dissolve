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
    auto appliedSize = requestedSize.isValid() ? requestedSize : QSize(16, 16);

    if (id.startsWith(QString("moduleLayer/")))
    {
        // Split the id
        auto parts = id.split("/");
        if (parts.size() != 3)
            return {};

        // Row of moduleLayer in the ModuleLayersModel
        auto moduleLayerRow = parts[1].toInt();
        // Row of the module in the ModuleLayerModel belonging to the parent ModuleLayersModel
        auto moduleRow = parts[2].toInt();

        auto moduleLayerIndex = dissolveModel_->moduleLayersModel()->index(moduleLayerRow, 0);
        if (!moduleLayerIndex.isValid())
            return {};

        // Get the ModuleLayerModel belonging to the parent ModuleLayersModel
        auto moduleLayerModel =
            dissolveModel_->moduleLayersModel()->data(moduleLayerIndex, Qt::UserRole + 1).value<ModuleLayerModel *>();

        auto moduleIndex = moduleLayerModel->index(moduleRow, 0);
        if (!moduleIndex.isValid())
            return {};

        // Update the size
        *size = appliedSize;

        // Get the icon as a QImage
        return moduleLayerModel->data(moduleIndex, Qt::DecorationRole).value<QIcon>().pixmap(QSize(appliedSize)).toImage();
    }
    return {};
}