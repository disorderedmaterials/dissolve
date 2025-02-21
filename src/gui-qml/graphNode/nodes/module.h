// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base.h"

class Module : public Node
{
    public:
    Module() : Node(typeid(this)) {};

};