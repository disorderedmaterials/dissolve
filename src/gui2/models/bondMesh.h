// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/legacy/configurationModel.h"
#include <QQuick3DGeometry>

class BondMesh : public QQuick3DGeometry
{
    Q_OBJECT
    Q_PROPERTY(const Configuration *configuration READ configuration WRITE setConfiguration);

    private:
    const Configuration *config_;

    public:
    const Configuration *configuration() const;
    void setConfiguration(const Configuration *config);
};
