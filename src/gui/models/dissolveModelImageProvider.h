// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <QQuickImageProvider>

// Forward Declarations
class DissolveModel;

class DissolveModelImageProvider : public QQuickImageProvider
{

    public:
    DissolveModelImageProvider(DissolveModel *dissolveModel);

    private:
    DissolveModel *dissolveModel_;

    public:
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};