// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/signals.h"
#include <gtest/gtest.h>

TEST(KeywordSignalsTest, KeywordSignals)
{
    KeywordSignals signals, signals2;

    // Add single signal
    signals += KeywordSignals::ReloadExternalData;
    EXPECT_TRUE(signals.isSet(KeywordSignals::ReloadExternalData));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::ReloadExternalData));

    // Reset by int assignment
    signals = 0;
    EXPECT_FALSE(signals.isSet(KeywordSignals::ReloadExternalData));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::ReloadExternalData));

    // Add multiple signals
    signals += KeywordSignals::ClearData;
    signals += KeywordSignals::ReloadExternalData;
    signals += KeywordSignals::RecreateRenderables;
    EXPECT_TRUE(signals.isSet(KeywordSignals::ClearData));
    EXPECT_TRUE(signals.isSet(KeywordSignals::RecreateRenderables));
    EXPECT_TRUE(signals.isSet(KeywordSignals::ReloadExternalData));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::ClearData));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::RecreateRenderables));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::ReloadExternalData));

    // Remove one, test again
    signals -= KeywordSignals::RecreateRenderables;
    EXPECT_TRUE(signals.isSet(KeywordSignals::ClearData));
    EXPECT_FALSE(signals.isSet(KeywordSignals::RecreateRenderables));
    EXPECT_TRUE(signals.isSet(KeywordSignals::ReloadExternalData));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::ClearData));
    EXPECT_FALSE(signals.isSetOrNone(KeywordSignals::RecreateRenderables));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::ReloadExternalData));

    // Convert to int, clear, set, and test again
    int i = signals;
    signals = 0;
    signals = i;
    EXPECT_TRUE(signals.isSet(KeywordSignals::ClearData));
    EXPECT_FALSE(signals.isSet(KeywordSignals::RecreateRenderables));
    EXPECT_TRUE(signals.isSet(KeywordSignals::ReloadExternalData));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::ClearData));
    EXPECT_FALSE(signals.isSetOrNone(KeywordSignals::RecreateRenderables));
    EXPECT_TRUE(signals.isSetOrNone(KeywordSignals::ReloadExternalData));

    // Combine integers
    signals = 0;
    signals += KeywordSignals::ClearData;
    signals2 = 0;
    signals2 += KeywordSignals::ReloadExternalData;
    EXPECT_TRUE(signals.isSet(KeywordSignals::ClearData));
    EXPECT_FALSE(signals.isSet(KeywordSignals::RecreateRenderables));
    EXPECT_FALSE(signals.isSet(KeywordSignals::ReloadExternalData));
    EXPECT_FALSE(signals2.isSet(KeywordSignals::ClearData));
    EXPECT_FALSE(signals2.isSet(KeywordSignals::RecreateRenderables));
    EXPECT_TRUE(signals2.isSet(KeywordSignals::ReloadExternalData));
    i = signals2;
    signals += i;
    EXPECT_TRUE(signals.isSet(KeywordSignals::ClearData));
    EXPECT_FALSE(signals.isSet(KeywordSignals::RecreateRenderables));
    EXPECT_TRUE(signals.isSet(KeywordSignals::ReloadExternalData));
}
