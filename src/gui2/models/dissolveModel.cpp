// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/dissolveModel.h"
#include "nodes/dissolve.h"

// Return the current filename
QUrl DissolveModel::fileName() const { return QUrl(dissolveGraph_->inputFile().c_str()); }

void DissolveModel::loadInput(QUrl filename)
{
    dissolveGraph_->loadTOML(filename.toLocalFile().toStdString());
    Q_EMIT(modelsUpdated());
}

DissolveGraph *DissolveModel::dissolveGraph()
{
    if (!dissolveGraph_)
        Messenger::exception("DissolveModel is lacking a backend.  This should *never* happen, so please contact the "
                             "Dissolve developers to inform them of the issue.");
    return dissolveGraph_;
}

// Set DissolveGraph root
void DissolveModel::setDissolveGraph(DissolveGraph *dissolveGraph)
{
    dissolveGraph_ = dissolveGraph;

    Q_EMIT modelsUpdated();
}

// Update models
void DissolveModel::update() { Q_EMIT modelsUpdated(); }

bool DissolveModel::saveAs(QUrl filename) { return dissolveGraph_->saveTOML(filename.toLocalFile().toStdString()); }
