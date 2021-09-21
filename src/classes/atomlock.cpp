// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include <classes/atomlock.h>

AtomLock::AtomLock(Configuration *parent) : parent_(parent){};

AtomLock::~AtomLock()
{
    for (auto &mol : parent_->molecules())
        mol->updateAtoms(parent_->atoms());
    parent_->cells().updateAtoms(parent_->atoms());
};
