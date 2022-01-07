// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <bitset>

// Keyword Signals
class KeywordSignals
{
    public:
    enum KeywordSignal
    {
        ClearData,
        RecreateRenderables,
        ReloadExternalData,
        nKeywordSignals
    };
    KeywordSignals();
    ~KeywordSignals() = default;
    explicit KeywordSignals(int signalMask);
    KeywordSignals(const KeywordSignals &source);
    KeywordSignals &operator=(int signalMask);
    void operator+=(KeywordSignals::KeywordSignal signal);
    void operator+=(int signalMask);
    void operator-=(KeywordSignals::KeywordSignal signal);
    operator int() const;

    private:
    // Signals to be emitted (via Qt) when editing this keyword in the GUI
    std::bitset<nKeywordSignals> signalMask_;

    public:
    // Return whether any signals are set
    bool anySet() const;
    // Return true if the specified signal is set
    bool isSet(KeywordSignal keywordSignal) const;
    // Return true if the specified signal is set, or if none are set at all
    bool isSetOrNone(KeywordSignal keywordSignal) const;
};
