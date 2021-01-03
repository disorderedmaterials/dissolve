// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectforcefielddialog.h"
#include "templates/list.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Forcefield;
class SelectForcefieldWidget;

// Select Forcefield Dialog
class SelectForcefieldDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectForcefieldDialog(QWidget *parent, const List<Forcefield> &forcefields);
    ~SelectForcefieldDialog();

    private:
    // Main form declaration
    Ui::SelectForcefieldDialog ui_;

    private slots:
    void on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid);
    void on_ForcefieldWidget_forcefieldDoubleClicked();
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    public:
    // Run the dialog, returning the selected Forcefield
    std::shared_ptr<Forcefield> selectForcefield(std::shared_ptr<Forcefield> currentFF);
};
