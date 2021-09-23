// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <classes/configuration.h>

class AtomLock
{
    public:
    AtomLock(Configuration &parent);
    ~AtomLock();

    private:
    Configuration &parent_;
};
