// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/plot/plotModelBase.h"

PlotModelBase::PlotModelBase(int dimensions, QObject *parent) : QAbstractListModel(parent), N(dimensions)
{
    QObject::connect(this, &PlotModelBase::contentChanged, this, [this]() { ++version_; });
}

//
int PlotModelBase::version() { return version_; }