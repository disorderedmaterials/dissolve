// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/models/atomTypeModel.h"
#include "gui/models/masterTermTreeModel.h"
#include "gui/ui_addforcefieldtermsdialog.h"
#include "gui/wizard.hui"
#include "main/dissolve.h"

// Add Forcefield Terms Dialog
class AddForcefieldTermsDialog : public QDialog
{
    Q_OBJECT

    public:
    AddForcefieldTermsDialog(QWidget *parent);
    ~AddForcefieldTermsDialog() = default;
};
