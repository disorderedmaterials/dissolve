// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "gui/keywordWidgets/optionalInt.h"
#include <QApplication>
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(OptionalIntegerTest, Widget)
{
    char arg0[] = "OptionalIntegerTest";
    char arg1[] = "-platform";
    char arg2[] = "offscreen";
    char *argv[] = {arg0, arg1, arg2};
    int argc{3};
    QApplication app(argc, argv);

    // Minimum at -50, maximum at 50
    std::optional<int> value_;
    OptionalIntegerKeyword keyword(value_, -50, 50, 1, "Null Text");
    CoreData coreData_;
    OptionalIntegerKeywordWidget widget(nullptr, &keyword, coreData_);

    // Null value_ to start with
    EXPECT_TRUE(!value_);
    // Step the spinbox up
    widget.stepBy(10);
    EXPECT_EQ(value_.value(), -40);
    // Direct set
    widget.setValue(12);
    EXPECT_EQ(value_.value(), 12);
    // Step the spinbox down
    widget.stepBy(-10);
    EXPECT_EQ(value_.value(), 2);
    // Up to above the maximum
    widget.stepBy(1e3);
    EXPECT_EQ(value_.value(), 50);
    // And back down, far enough to hit the minimum and nullify the value
    widget.stepBy(-1e6);
    EXPECT_TRUE(!value_);
    // Check the text in the spinbox
    EXPECT_STREQ("Null Text", qPrintable(widget.text()));
}

} // namespace UnitTest
