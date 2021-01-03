// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectsystemtemplatedialog.h"
#include "templates/list.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class SystemTemplate;

// Select System Template Dialog
class SelectSystemTemplateDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectSystemTemplateDialog(QWidget *parent, const List<SystemTemplate> &systemTemplates);
    ~SelectSystemTemplateDialog();

    private:
    // Main form declaration
    Ui::SelectSystemTemplateDialog ui_;
    // Available system templates
    const List<SystemTemplate> &systemTemplates_;

    private:
    // Update the list of system templates, optionally filtering them by name and description
    void updateTemplatesList(QString filter = QString());

    private slots:
    void on_FilterEdit_textChanged(const QString &text);
    void on_TemplatesList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_TemplatesList_itemDoubleClicked(QListWidgetItem *item);
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    public:
    // Run the dialog, returning the selected SystemTemplate
    SystemTemplate *selectTemplate();
};
