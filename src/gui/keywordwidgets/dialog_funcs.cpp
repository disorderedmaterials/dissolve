// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/keywordwidgets/dialog.h"

KeywordsDialog::KeywordsDialog(QWidget *parent, KeywordStore &keywords, const CoreData &coreData) : coreData_(coreData)
{
    ui_.setupUi(this);

    // We can only display a single group of widgets at present, so check the size of the index
    auto &&[keywordIndex, keywordMap] = keywords.keywordOrganisation();
    if (keywordIndex.size() > 1)
        Messenger::warn("There are {} keyword groups defined, but only one can be displayed. Tell the developer!\n");

    if (!keywordIndex.empty())
    {
        ui_.Keywords->setUp(keywordIndex.front(), keywordMap, coreData);

        connect(ui_.Keywords, SIGNAL(keywordChanged(int)), this, SLOT(keywordChanged(int)));
    }

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
