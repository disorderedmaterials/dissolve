// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/keywordwidgets/dialog.h"

KeywordsDialog::KeywordsDialog(QWidget *parent, const KeywordList &keywords, const CoreData &coreData) : coreData_(coreData)
{
    ui_.setupUi(this);

    ui_.Keywords->setUp(keywords, coreData);

    connect(ui_.Keywords, SIGNAL(dataModified()), this, SLOT(keywordChanged()));
    connect(ui_.Keywords, SIGNAL(setUpRequired()), this, SLOT(setUpRequired()));

    keywordsModified_ = false;
    setUpRequired_ = false;
}

KeywordsDialog::~KeywordsDialog() {}

// Run the dialog
void KeywordsDialog::showOptions()
{
    keywordsModified_ = false;
    setUpRequired_ = false;

    exec();
}

// Return whether any keywords have been modified in the current 'show'
bool KeywordsDialog::keywordsModified() const { return keywordsModified_; }

// Return whether any set-up needs to be re-run following keyword modification
bool KeywordsDialog::isSetUpRequired() const { return setUpRequired_; }

/*
 * Slots
 */

void KeywordsDialog::keywordChanged() { keywordsModified_ = true; }

void KeywordsDialog::setUpRequired() { setUpRequired_ = true; }

void KeywordsDialog::on_OKButton_clicked(bool checked) { accept(); }
