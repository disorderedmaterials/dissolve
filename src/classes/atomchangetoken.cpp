// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include <classes/atomchangetoken.h>

AtomChangeToken::AtomChangeToken(Configuration &parent) : parent_(parent){};

AtomChangeToken::~AtomChangeToken()
{
    for (auto &mol : parent_.molecules())
        mol->updateAtoms(parent_.atoms());
    parent_.cells().updateAtoms(parent_.atoms());
};
