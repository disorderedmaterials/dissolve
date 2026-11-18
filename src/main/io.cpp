// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/serialiser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "main/compatibility.h"
#include "main/dissolve.h"
#include "main/version.h"
#include "nodes/dissolve.h"
#include <fstream>

// Return whether an input filename has been set
bool Dissolve::hasInputFilename() const { return (!inputFilename_.empty()); }

// Set current input filename
void Dissolve::setInputFilename(std::string_view filename)
{
    inputFilename_ = filename;

    restartFilename_ = std::format("{}.restart", inputFilename_);
}

// Return current input filename
std::string_view Dissolve::inputFilename() const { return inputFilename_; }

// Return restart filename
std::string_view Dissolve::restartFilename() const { return restartFilename_; }

// Return whether a restart filename has been set
bool Dissolve::hasRestartFilename() const { return (!restartFilename_.empty()); }
