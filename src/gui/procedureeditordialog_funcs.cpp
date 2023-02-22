// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/coredata.h"
#include "data/ff/ff.h"
#include "gui/procedureeditordialog.h"

ProcedureEditorDialog::ProcedureEditorDialog(QWidget *parent, DissolveWindow *dissolveWindow, const Procedure &proc)
{
    ui_.setupUi(this);

    // Copy the supplied procedure
    procedure_.deserialise(proc.serialise());
    ui_.ProcedureViewer->setUp(dissolveWindow, procedure_);

    // Set up the available nodes tree
    ui_.AvailableNodesTree->setModel(&nodePaletteModel_);
    ui_.AvailableNodesTree->expandAll();
}

void ProcedureEditorDialog::on_OKButton_clicked(bool checked) { accept(); }

void ProcedureEditorDialog::on_CancelButton_clicked(bool checked) { reject(); }

// Run the dialog, returning whether the procedure was changed
bool ProcedureEditorDialog::executeEdit() {}