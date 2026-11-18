// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/dissolveModel.h"
#include "nodes/dissolve.h"

/*
 * Data
 */

Dissolve &DissolveModel::dissolve()
{
    if (!dissolve_)
        Messenger::exception("DissolveModel is lacking a backend.  This should *never* happen, so please contact the "
                             "Dissolve developers to inform them of the issue.");
    return *dissolve_;
}

// Set reference to Dissolve
void DissolveModel::setDissolve(Dissolve &dissolve)
{
    dissolve_ = &dissolve;

    Q_EMIT modelsUpdated();
}

// Update models
void DissolveModel::update() { Q_EMIT modelsUpdated(); }

// Getter for filename
QUrl DissolveModel::fileName() const { return QUrl(dissolve_->inputFilename().data()); }

void DissolveModel::loadInput(QUrl filename)
{
    dissolve_->loadTOML(filename.toLocalFile().toStdString());
    Q_EMIT(modelsUpdated());
}

bool DissolveModel::saveAs(QUrl filename) { return dissolve_->saveTOML(filename.toLocalFile().toStdString()); }

Graph *DissolveModel::graph()
{
    if (!dissolve_)
        return nullptr;
    return dissolve_->graph();
}
