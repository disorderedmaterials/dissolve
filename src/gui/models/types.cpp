// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/types.h"
#include "gui/models/atomTypeModel.h"
#include "gui/models/configurationModel.h"
#include "gui/models/dissolveModel.h"
#include "gui/models/modifyChargesModel.h"
#include "gui/models/nodeGraph/graphModel.h"
#include "gui/models/simpleForcefieldModel.h"
#include "gui/models/sortFilterProxy.h"
#include "gui/models/speciesModel.h"
#include <QQmlEngine>
#include <QSortFilterProxyModel>

void Types::registerDissolveQmlTypes()
{
    qmlRegisterType<DissolveModel>(PROJECT, 1, 0, "DissolveModel");
    qmlRegisterType<SpeciesModel>(PROJECT, 1, 0, "SpeciesModel");
    qmlRegisterType<ConfigurationModel>(PROJECT, 1, 0, "ConfigurationModel");
    qmlRegisterType<AtomTypeModel>(PROJECT, 1, 0, "AtomTypeModel");
    qmlRegisterType<CommonAngleModel>(PROJECT, 1, 0, "CommonAngleModel");
    qmlRegisterType<CommonBondModel>(PROJECT, 1, 0, "CommonBondModel");
    qmlRegisterType<CommonImproperModel>(PROJECT, 1, 0, "CommonImproperModel");
    qmlRegisterType<CommonTorsionModel>(PROJECT, 1, 0, "CommonTorsionModel");
    qmlRegisterType<ModifyChargesModel>(PROJECT, 1, 0, "ModifyChargesModel");
    qmlRegisterType<SortFilterProxy>(PROJECT, 1, 0, "SortFilterProxy");
    qmlRegisterType<GraphModel>(PROJECT, 1, 0, "GraphModel");
    qmlRegisterType<SimpleForcefieldModel>(PROJECT, 1, 0, "SimpleForcefieldModel");
}
