// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/addForcefieldDialogModel.h"
#include "gui/models/dissolveModel.h"
#include "gui/types.h"
#include <QQmlContext>

void registerDissolveQmlTypes() const
{

    qmlRegisterType<AddForcefieldDialogModel>(PROJECT, 1, 0, "AddForcefieldDialogModel");
    qmlRegisterType<DissolveModel>(PROJECT, 1, 0, "DissolveModel");
    qmlRegisterType<SpeciesModel>(PROJECT, 1, 0, "SpeciesModel");
    qmlRegisterType<ConfigurationModel>(PROJECT, 1, 0, "ConfigurationModel");
    qmlRegisterType<ModuleLayersModel>(PROJECT, 1, 0, "ModuleLayersModel");
    qmlRegisterType<AtomTypeModel>(PROJECT, 1, 0, "AtomTypeModel");
    qmlRegisterType<MasterAngleModel>(PROJECT, 1, 0, "MasterAngleModel");
    qmlRegisterType<MasterBondModel>(PROJECT, 1, 0, "MasterBondModel");
    qmlRegisterType<MasterImproperModel>(PROJECT, 1, 0, "MasterImproperModel");
    qmlRegisterType<MasterTorsionModel>(PROJECT, 1, 0, "MasterTorsionModel");
}