// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectElementDialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Element;
class SelectElementWidget;

// Select Element Dialog
class SelectElementDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectElementDialog(QWidget *parent);
    ~SelectElementDialog();

    private:
    // Main form declaration
    Ui::SelectElementDialog ui_;

    private Q_SLOTS:
    void on_ElementWidget_elementSelected(bool dummy);
    void on_ElementWidget_elementDoubleClicked();
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    public:
    // Run the dialog, returning the selected Element
    Elements::Element selectElement(Elements::Element currentElement);
};
