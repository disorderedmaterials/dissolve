// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/dataManagerSimulationModel.h"
#include "items/list.h"
#include <QDialog>
#include <QQuickWidget>
#include <vector>

// Forward Declarations
class Dissolve;

// Data Manager Dialog
class DataManagerDialog : public QDialog
{
    Q_OBJECT

    public:
    DataManagerDialog(QWidget *parent, Dissolve &dissolve, GenericList &items);
    ~DataManagerDialog() = default;

    private:
    // Reference to Dissolve
    Dissolve &dissolve_;
    // Simulation Model
    DataManagerSimulationModel simModel_;

    /*
     * UI
     */
    private:
    QQuickWidget *view_{nullptr};
};
