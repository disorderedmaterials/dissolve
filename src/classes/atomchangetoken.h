// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"

class AtomChangeToken
{
    public:
    AtomChangeToken(Configuration &parent);
    ~AtomChangeToken();

    private:
    Configuration &parent_;
};
