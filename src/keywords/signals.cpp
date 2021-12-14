// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/signals.h"

KeywordSignals::KeywordSignals() : signalMask_(0) {}

KeywordSignals::KeywordSignals(int signalMask) : signalMask_(signalMask) {}

KeywordSignals::KeywordSignals(const KeywordSignals &source) { signalMask_ = source.signalMask_; }

KeywordSignals &KeywordSignals::operator=(int signalMask)
{
    signalMask_ = signalMask;
    return *this;
}

void KeywordSignals::operator+=(int signalMask) { signalMask_ |= signalMask; }

KeywordSignals::operator int() const { return signalMask_; }

// Return true if the specified signal is set
bool KeywordSignals::set(KeywordSignal keywordSignal) const { return signalMask_ & keywordSignal; }

// Return true if the specified signal is set, or if none are set at all
bool KeywordSignals::setOrNull(KeywordSignal keywordSignal) const { return signalMask_ == 0 || (signalMask_ & keywordSignal); }
