// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/types.h"
#include "gui/models/addForcefieldDialogModel.h"
#include "gui/models/atomTypeModel.h"
#include "gui/models/configurationModel.h"
#include "gui/models/dataManagerSimulationModel.h"
#include "gui/models/dissolveModel.h"
#include "gui/models/masterTermTreeModel.h"
#include "gui/models/modifyChargesModel.h"
#include "gui/models/moduleLayersModel.h"
#include "gui/models/sortFilterProxy.h"
#include "gui/models/speciesModel.h"
#include <QQmlEngine>
#include <QSortFilterProxyModel>

void Types::registerDissolveQmlTypes()
{
    qmlRegisterType<AddForcefieldDialogModel>(PROJECT, 1, 0, "AddForcefieldDialogModel");
    qmlRegisterType<DataManagerSimulationModel>(PROJECT, 1, 0, "DataManagerSimulationModel");
    qmlRegisterType<DissolveModel>(PROJECT, 1, 0, "DissolveModel");
    qmlRegisterType<SpeciesModel>(PROJECT, 1, 0, "SpeciesModel");
    qmlRegisterType<ConfigurationModel>(PROJECT, 1, 0, "ConfigurationModel");
    qmlRegisterType<ModuleLayersModel>(PROJECT, 1, 0, "ModuleLayersModel");
    qmlRegisterType<AtomTypeModel>(PROJECT, 1, 0, "AtomTypeModel");
    qmlRegisterType<MasterAngleModel>(PROJECT, 1, 0, "MasterAngleModel");
    qmlRegisterType<MasterBondModel>(PROJECT, 1, 0, "MasterBondModel");
    qmlRegisterType<MasterImproperModel>(PROJECT, 1, 0, "MasterImproperModel");
    qmlRegisterType<MasterTorsionModel>(PROJECT, 1, 0, "MasterTorsionModel");
    qmlRegisterType<ModifyChargesModel>(PROJECT, 1, 0, "ModifyChargesModel");
    qmlRegisterType<SortFilterProxy>(PROJECT, 1, 0, "SortFilterProxy");
}
