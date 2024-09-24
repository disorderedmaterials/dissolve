// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "graphModel.h"
#include <qnamespace.h>
#include <qvariant.h>

GraphModelBase::GraphModelBase() {}

QAbstractListModel *GraphModelBase::edges() {return nullptr;}
QAbstractListModel *GraphModelBase::nodes() {return nullptr;}
