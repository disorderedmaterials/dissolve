// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Keyword Signals
class KeywordSignals
{
    public:
    KeywordSignals(int signalMask = 0);
    operator int() const;
    ~KeywordSignals() = default;

    public:
    enum KeywordSignal
    {
        ReloadExternalData = 1
    };

    private:
    // Signals to be emitted (via Qt) when editing this keyword in the GUI
    int signalMask_{0};

    public:
    // Return true if the specified signal is set, or if none are set at all
    bool setOrNull(KeywordSignal keywordSignal) const;
};
