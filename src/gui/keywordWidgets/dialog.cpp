// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/dialog.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "keywords/organiser.h"

KeywordsDialog::KeywordsDialog(QWidget *parent, const KeywordStore &keywordStore, CoreData &coreData) : coreData_(coreData)
{
    ui_.setupUi(this);

    if (keywordStore.nVisibleKeywords() == 0)
        return;

    // We can only display a single section of widgets at present
    if (keywordStore.sections().size() != 1)
        Messenger::warn("There are {} keyword sections defined, but only one can be displayed. Tell the developer!\n");

    ui_.Keywords->setUp(keywordStore.sections().front(), coreData);

    connect(ui_.Keywords, SIGNAL(keywordChanged(int)), this, SLOT(keywordChanged(int)));
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
Flags<KeywordBase::KeywordSignal> KeywordsDialog::keywordSignalMask() const { return keywordSignalMask_; }

/*
 * Slots
 */

void KeywordsDialog::keywordChanged(int signalMask)
{
    keywordsModified_ = true;
    keywordSignalMask_ += signalMask;
}

void KeywordsDialog::on_OKButton_clicked(bool checked) { accept(); }
