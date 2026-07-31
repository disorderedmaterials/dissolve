// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "simpleForcefieldModel.h"
#include "data/ff/library.h"
#include "nodes/forcefield.h"
#include <qcontainerfwd.h>

QString SimpleForcefieldModel::name()
{
    if (!ff_)
        return "No forcefield chosen";
    return QString::fromStdString(std::string(ff_->name()));
}

void SimpleForcefieldModel::setName(QString name) { ff_ = ForcefieldLibrary::forcefield(name.toStdString()); }

GraphModel *SimpleForcefieldModel::graphModel() { return graphModel_; }

void SimpleForcefieldModel::assignModel(GraphModel *graphModel) { graphModel_ = graphModel; }

QStringList SimpleForcefieldModel::library()
{
    QStringList result;
    for (auto ff : ForcefieldLibrary::forcefields())
    {
        result << QString::fromStdString(std::string(ff->name()));
    }
    return result;
}

// Finalise the node
void SimpleForcefieldModel::create(int x, int y)
{
    auto name = ff_->name();
    graphModel_->emplace_back(x, y, "Forcefield", std::string(name));
    auto ff = dynamic_cast<ForcefieldNode *>(graphModel_->graph()->findNode(name));
    // if (ff)
    // ff->forcefield() = ff_;
    // TODO DISSOLVE2
}
