// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Keyword Signals
class KeywordSignals
{
    public:
    KeywordSignals();
    explicit KeywordSignals(int signalMask);
    KeywordSignals(const KeywordSignals &source);
    KeywordSignals &operator=(int signalMask);
    void operator+=(int signalMask);
    operator int() const;
    ~KeywordSignals() = default;

    public:
    enum KeywordSignal
    {
        ClearData = 1,
        RecreateRenderables = 2,
        ReloadExternalData = 4
    };

    private:
    // Signals to be emitted (via Qt) when editing this keyword in the GUI
    int signalMask_{0};

    public:
    // Return true if the specified signal is set
    bool set(KeywordSignal keywordSignal) const;
    // Return true if the specified signal is set, or if none are set at all
    bool setOrNull(KeywordSignal keywordSignal) const;
};
