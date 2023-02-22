// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/models/nodePaletteModel.h"
#include "gui/models/procedureModel.h"
#include "gui/ui_procedureeditordialog.h"
#include "procedure/procedure.h"
#include <QDialog>

// Forward Declarations
class Dissolve;

// Procedure Editor Dialog
class ProcedureEditorDialog : public QDialog
{
    Q_OBJECT

    public:
    ProcedureEditorDialog(QWidget *parent, DissolveWindow *dissolveWindow, const Procedure &proc);
    ~ProcedureEditorDialog() = default;

    private:
    // Main form declaration
    Ui::ProcedureEditorDialog ui_;
    // Model for node palette
    NodePaletteModel nodePaletteModel_;

    // Procedure being edited
    Procedure procedure_;

    private slots:
    void on_OKButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    public:
    // Run the dialog, returning whether the procedure was changed
    bool executeEdit();
};
