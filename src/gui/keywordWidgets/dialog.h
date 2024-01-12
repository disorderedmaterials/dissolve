// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/ui_dialog.h"
#include "keywords/base.h"
#include <QDialog>

// Forward Declarations
class CoreData;
class KeywordStoreOrganiser;

// Keywords Dialog
class KeywordsDialog : public QDialog
{
    Q_OBJECT

    public:
    KeywordsDialog(QWidget *parent, const KeywordStore &keywordStore, CoreData &coreData);
    ~KeywordsDialog() = default;

    private:
    // Main form declaration
    Ui::KeywordsDialog ui_;
    // CoreData reference
    CoreData &coreData_;
    // Whether any keywords have been modified in the current 'show'
    bool keywordsModified_{false};
    // Combined signal mask for all keyword changes
    Flags<KeywordBase::KeywordSignal> keywordSignalMask_;

    public:
    // Run the dialog
    void showOptions();
    // Return whether any keywords have been modified in the current 'show'
    bool keywordsModified() const;
    // Return combined signal mask for all keyword changes
    Flags<KeywordBase::KeywordSignal> keywordSignalMask() const;

    /*
     * Slots
     */
    private Q_SLOTS:
    void keywordChanged(int signalMask);
    void on_OKButton_clicked(bool checked);
};
