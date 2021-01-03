// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/ui_dialog.h"
#include <QDialog>

// Forward Declarations
class CoreData;
class KeywordList;

// Keywords Dialog
class KeywordsDialog : public QDialog
{
    Q_OBJECT

    public:
    KeywordsDialog(QWidget *parent, const KeywordList &keywords, const CoreData &coreData);
    ~KeywordsDialog();

    private:
    // Main form declaration
    Ui::KeywordsDialog ui_;
    // CoreData reference
    const CoreData &coreData_;
    // Whether any keywords have been modified in the current 'show'
    bool keywordsModified_;
    // Whether any set-up needs to be re-run following keyword modification
    bool setUpRequired_;

    public:
    // Run the dialog
    void showOptions();
    // Return whether any keywords have been modified in the current 'show'
    bool keywordsModified() const;
    // Return whether any set-up needs to be re-run following keyword modification
    bool isSetUpRequired() const;

    /*
     * Slots
     */
    private slots:
    void keywordChanged();
    void setUpRequired();
    void on_OKButton_clicked(bool checked);
};
