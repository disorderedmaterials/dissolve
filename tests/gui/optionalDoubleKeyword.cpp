// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "gui/keywordWidgets/optionalDouble.hui"
#include <QApplication>
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(OptionalDoubleTest, Widget)
{
    char arg0[] = "OptionalDoubleTest";
    char arg1[] = "-platform";
    char arg2[] = "offscreen";
    char *argv[] = {arg0, arg1, arg2};
    int argc{3};
    QApplication app(argc, argv);

    // Minimum at -50.0, maximum at 50.0
    std::optional<double> value_;
    OptionalDoubleKeyword keyword(value_, -50.0, 50.0, 0.8, "Null Text");
    CoreData coreData_;
    OptionalDoubleKeywordWidget widget(nullptr, &keyword, coreData_);

    // Null value_ to start with
    EXPECT_TRUE(!value_);
    // Step the spinbox up
    widget.stepBy(10);
    EXPECT_NEAR(value_.value(), -42.0, 1.0e-8);
    // Direct set
    widget.setValue(1.2345);
    EXPECT_NEAR(value_.value(), 1.2345, 1.0e-8);
    // Step the spinbox down
    widget.stepBy(-10);
    EXPECT_NEAR(value_.value(), 1.2345 - 8.0, 1.0e-8);
    // Up to above the maximum
    widget.stepBy(1.0e3);
    EXPECT_NEAR(value_.value(), 50.0, 1.0e-8);
    // And back down, far enough to hit the minimum and nullify the value
    widget.stepBy(-1.0e6);
    EXPECT_TRUE(!value_);
    // Check the text in the spinbox
    EXPECT_STREQ("Null Text", qPrintable(widget.text()));
}

} // namespace UnitTest
