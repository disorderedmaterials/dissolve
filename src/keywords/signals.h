// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <bitset>

// Keyword Signals
class KeywordSignals
{
    public:
    KeywordSignals();
    ~KeywordSignals() = default;
    explicit KeywordSignals(int signalMask);
    KeywordSignals(const KeywordSignals &source);
    KeywordSignals &operator=(int signalMask);
    void operator+=(int signalMask);
    operator int() const;

    public:
    enum KeywordSignal
    {
        ClearData,
        RecreateRenderables,
        ReloadExternalData,
        nKeywordSignals
    };

    private:
    // Signals to be emitted (via Qt) when editing this keyword in the GUI
    std::bitset<nKeywordSignals> signalMask_;

    public:
    // Return true if the specified signal is set
    bool set(KeywordSignal keywordSignal) const;
    // Return true if the specified signal is set, or if none are set at all
    bool setOrNone(KeywordSignal keywordSignal) const;
};
