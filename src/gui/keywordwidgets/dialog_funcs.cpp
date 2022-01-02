// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/keywordwidgets/dialog.h"

KeywordsDialog::KeywordsDialog(QWidget *parent, KeywordStore &keywords, const CoreData &coreData) : coreData_(coreData)
{
    ui_.setupUi(this);

    ui_.Keywords->setUp(keywords, coreData);

    connect(ui_.Keywords, SIGNAL(keywordChanged(int)), this, SLOT(keywordChanged(int)));

    keywordsModified_ = false;
}

// Run the dialog
void KeywordsDialog::showOptions()
{
    keywordsModified_ = false;
    keywordSignalMask_ = 0;

    exec();
}

// Return whether any keywords have been modified in the current 'show'
bool KeywordsDialog::keywordsModified() const { return keywordsModified_; }

// Return combined signal mask for all keyword changes
KeywordSignals KeywordsDialog::keywordSignalMask() const { return keywordSignalMask_; }

/*
 * Slots
 */

void KeywordsDialog::keywordChanged(int signalMask)
{
    keywordsModified_ = true;
    keywordSignalMask_ += signalMask;
}

void KeywordsDialog::on_OKButton_clicked(bool checked) { accept(); }
