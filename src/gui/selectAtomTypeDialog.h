// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectAtomTypeDialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class AtomType;
class SelectAtomTypeWidget;

// Select AtomType Dialog
class SelectAtomTypeDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectAtomTypeDialog(QWidget *parent, const CoreData &coreData, QString dialogTitle);
    ~SelectAtomTypeDialog() = default;

    private:
    // Main form declaration
    Ui::SelectAtomTypeDialog ui_;

    private slots:
    void on_AtomTypeWidget_atomTypeSelectionChanged(bool isValid);
    void on_AtomTypeWidget_atomTypeDoubleClicked();
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    public:
    // Run the dialog, returning a single selected AtomType
    const std::shared_ptr<AtomType> selectAtomType(std::optional<Elements::Element> filterElement = std::nullopt);
};
