// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/addForcefieldDialogModel.h"
#include "gui/models/forcefieldModel.h"
#include "gui/models/masterTermTreeModel.h"
#include "main/dissolve.h"
#include <QDialog>

// Add Forcefield Terms Dialog
class AddForcefieldTermsDialog : public QDialog
{
    Q_OBJECT
    public:
    AddForcefieldTermsDialog(QWidget *parent, Dissolve &dissolve, Species *sp);
    ~AddForcefieldTermsDialog() = default;
};
