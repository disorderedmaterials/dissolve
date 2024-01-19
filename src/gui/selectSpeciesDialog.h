// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/speciesFilterProxy.h"
#include "gui/ui_selectSpeciesDialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;
class SelectSpeciesWidget;

// Select Species Dialog
class SelectSpeciesDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectSpeciesDialog(QWidget *parent, const CoreData &coreData, QString dialogTitle);
    ~SelectSpeciesDialog() = default;

    private:
    // Main form declaration
    Ui::SelectSpeciesDialog ui_;

    private Q_SLOTS:
    void on_SpeciesWidget_speciesSelectionChanged(bool isValid);
    void on_SpeciesWidget_speciesDoubleClicked();
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    public:
    // Run the dialog, returning a single selected Species
    const Species *selectSingleSpecies(const Flags<SpeciesFilterProxy::FilterFlags> &flags = {});
    // Run the dialog, returning a list of selected Species
    std::vector<const Species *> selectSpecies(const Flags<SpeciesFilterProxy::FilterFlags> &flags = {}, int minSpecies = 1,
                                               std::optional<int> maxSpecies = std::nullopt);
};
