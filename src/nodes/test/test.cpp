// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include "nodes/test/test.h"
#include <cmath>

TestNode::TestNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "A configuration input", configuration_);
    addInput("CreateConfiguration", "Whether to create the optional configuration on run", createConfiguration_);
    addInput("Number", "A single number", number_);
    addInput("NumberVector", "A vector of numbers", numberVector_);
    addInput("OptionalNumber", "A single number", optionalNumber_);
    addInput("Variant", "A variant", variant_);
    addInput("Message", "A message", message_);
    addInput("Char", "A character", char_);
    addInput("CharPtr", "A character", charPtr_);

    // Outputs
    addOutput("Configuration", "A configuration output", configuration_);
    addOptionalPointerOutput<Configuration>("OptionalConfiguration", "An optional Configuration", optionalConfiguration_);
    addOutput("Number", "A single number", number_);
    addOutput("NumberVector", "A vector of numbers", numberVector_);
    addOutput("OptionalNumber", "An optional number", optionalNumber_);
    addOutput("Variant", "A variant", variant_);
    addOutput("Sine", "A test sine data series", sineData1D_);
    addOutput("Cosine", "A test cosine data series", cosineData1D_);
    addOutput("Sinusoid2D", "A test sine data series", sinusoidalData2D_);

    // Fill out some test data at construction time
    auto dx = M_PI / 4;

    sineData1D_.addPoint(0, sin(0));
    sineData1D_.addPoint(dx, sin(dx));
    sineData1D_.addPoint(2 * dx, sin(2 * dx));
    sineData1D_.addPoint(3 * dx, sin(3 * dx));
    sineData1D_.addPoint(4 * dx, sin(4 * dx));
    sineData1D_.addPoint(5 * dx, sin(5 * dx));
    sineData1D_.addPoint(6 * dx, sin(6 * dx));
    sineData1D_.addPoint(7 * dx, sin(7 * dx));
    sineData1D_.addPoint(8 * dx, sin(8 * dx));
    
    cosineData1D_.addPoint(0, cos(0));
    cosineData1D_.addPoint(dx, cos(dx));
    cosineData1D_.addPoint(2 * dx, cos(2 * dx));
    cosineData1D_.addPoint(3 * dx, cos(3 * dx));
    cosineData1D_.addPoint(4 * dx, cos(4 * dx));
    cosineData1D_.addPoint(5 * dx, cos(5 * dx));
    cosineData1D_.addPoint(6 * dx, cos(6 * dx));
    cosineData1D_.addPoint(7 * dx, cos(7 * dx));
    cosineData1D_.addPoint(8 * dx, cos(8 * dx));

    sinusoidalData2D_.initialise(0, 2 * M_PI, (2 * M_PI) / 16, 0, 4, 0.25);
    sinusoidalData2D_.value(0, 0) = cos(0*dx + (0*M_PI/8));
    sinusoidalData2D_.value(0, 1) = cos(0*dx + (1*M_PI/8));
    sinusoidalData2D_.value(0, 2) = cos(0*dx + (2*M_PI/8));
    sinusoidalData2D_.value(0, 3) = cos(0*dx + (3*M_PI/8));
    sinusoidalData2D_.value(0, 4) = cos(0*dx + (4*M_PI/8));
    sinusoidalData2D_.value(0, 5) = cos(0*dx + (5*M_PI/8));
    sinusoidalData2D_.value(0, 6) = cos(0*dx + (6*M_PI/8));
    sinusoidalData2D_.value(0, 7) = cos(0*dx + (7*M_PI/8));
    sinusoidalData2D_.value(0, 8) = cos(0*dx + (8*M_PI/8));
    sinusoidalData2D_.value(0, 9) = cos(0*dx + (9*M_PI/8));
    sinusoidalData2D_.value(0, 10) = cos(0*dx + (10*M_PI/8));
    sinusoidalData2D_.value(0, 11) = cos(0*dx + (11*M_PI/8));
    sinusoidalData2D_.value(0, 12) = cos(0*dx + (12*M_PI/8));
    sinusoidalData2D_.value(0, 13) = cos(0*dx + (13*M_PI/8));
    sinusoidalData2D_.value(0, 14) = cos(0*dx + (14*M_PI/8));
    sinusoidalData2D_.value(0, 15) = cos(0*dx + (15*M_PI/8));
    sinusoidalData2D_.value(1, 0) = cos(1*dx + (0*M_PI/8));
    sinusoidalData2D_.value(1, 1) = cos(1*dx + (1*M_PI/8));
    sinusoidalData2D_.value(1, 2) = cos(1*dx + (2*M_PI/8));
    sinusoidalData2D_.value(1, 3) = cos(1*dx + (3*M_PI/8));
    sinusoidalData2D_.value(1, 4) = cos(1*dx + (4*M_PI/8));
    sinusoidalData2D_.value(1, 5) = cos(1*dx + (5*M_PI/8));
    sinusoidalData2D_.value(1, 6) = cos(1*dx + (6*M_PI/8));
    sinusoidalData2D_.value(1, 7) = cos(1*dx + (7*M_PI/8));
    sinusoidalData2D_.value(1, 8) = cos(1*dx + (8*M_PI/8));
    sinusoidalData2D_.value(1, 9) = cos(1*dx + (9*M_PI/8));
    sinusoidalData2D_.value(1, 10) = cos(1*dx + (10*M_PI/8));
    sinusoidalData2D_.value(1, 11) = cos(1*dx + (11*M_PI/8));
    sinusoidalData2D_.value(1, 12) = cos(1*dx + (12*M_PI/8));
    sinusoidalData2D_.value(1, 13) = cos(1*dx + (13*M_PI/8));
    sinusoidalData2D_.value(1, 14) = cos(1*dx + (14*M_PI/8));
    sinusoidalData2D_.value(1, 15) = cos(1*dx + (15*M_PI/8));
    sinusoidalData2D_.value(2, 0) = cos(2*dx + (0*M_PI/8));
    sinusoidalData2D_.value(2, 1) = cos(2*dx + (1*M_PI/8));
    sinusoidalData2D_.value(2, 2) = cos(2*dx + (2*M_PI/8));
    sinusoidalData2D_.value(2, 3) = cos(2*dx + (3*M_PI/8));
    sinusoidalData2D_.value(2, 4) = cos(2*dx + (4*M_PI/8));
    sinusoidalData2D_.value(2, 5) = cos(2*dx + (5*M_PI/8));
    sinusoidalData2D_.value(2, 6) = cos(2*dx + (6*M_PI/8));
    sinusoidalData2D_.value(2, 7) = cos(2*dx + (7*M_PI/8));
    sinusoidalData2D_.value(2, 8) = cos(2*dx + (8*M_PI/8));
    sinusoidalData2D_.value(2, 9) = cos(2*dx + (9*M_PI/8));
    sinusoidalData2D_.value(2, 10) = cos(2*dx + (10*M_PI/8));
    sinusoidalData2D_.value(2, 11) = cos(2*dx + (11*M_PI/8));
    sinusoidalData2D_.value(2, 12) = cos(2*dx + (12*M_PI/8));
    sinusoidalData2D_.value(2, 13) = cos(2*dx + (13*M_PI/8));
    sinusoidalData2D_.value(2, 14) = cos(2*dx + (14*M_PI/8));
    sinusoidalData2D_.value(2, 15) = cos(2*dx + (15*M_PI/8));
    sinusoidalData2D_.value(3, 0) = cos(3*dx + (0*M_PI/8));
    sinusoidalData2D_.value(3, 1) = cos(3*dx + (1*M_PI/8));
    sinusoidalData2D_.value(3, 2) = cos(3*dx + (2*M_PI/8));
    sinusoidalData2D_.value(3, 3) = cos(3*dx + (3*M_PI/8));
    sinusoidalData2D_.value(3, 4) = cos(3*dx + (4*M_PI/8));
    sinusoidalData2D_.value(3, 5) = cos(3*dx + (5*M_PI/8));
    sinusoidalData2D_.value(3, 6) = cos(3*dx + (6*M_PI/8));
    sinusoidalData2D_.value(3, 7) = cos(3*dx + (7*M_PI/8));
    sinusoidalData2D_.value(3, 8) = cos(3*dx + (8*M_PI/8));
    sinusoidalData2D_.value(3, 9) = cos(3*dx + (9*M_PI/8));
    sinusoidalData2D_.value(3, 10) = cos(3*dx + (10*M_PI/8));
    sinusoidalData2D_.value(3, 11) = cos(3*dx + (11*M_PI/8));
    sinusoidalData2D_.value(3, 12) = cos(3*dx + (12*M_PI/8));
    sinusoidalData2D_.value(3, 13) = cos(3*dx + (13*M_PI/8));
    sinusoidalData2D_.value(3, 14) = cos(3*dx + (14*M_PI/8));
    sinusoidalData2D_.value(3, 15) = cos(3*dx + (15*M_PI/8));
    sinusoidalData2D_.value(4, 0) = cos(4*dx + (0*M_PI/8));
    sinusoidalData2D_.value(4, 1) = cos(4*dx + (1*M_PI/8));
    sinusoidalData2D_.value(4, 2) = cos(4*dx + (2*M_PI/8));
    sinusoidalData2D_.value(4, 3) = cos(4*dx + (3*M_PI/8));
    sinusoidalData2D_.value(4, 4) = cos(4*dx + (4*M_PI/8));
    sinusoidalData2D_.value(4, 5) = cos(4*dx + (5*M_PI/8));
    sinusoidalData2D_.value(4, 6) = cos(4*dx + (6*M_PI/8));
    sinusoidalData2D_.value(4, 7) = cos(4*dx + (7*M_PI/8));
    sinusoidalData2D_.value(4, 8) = cos(4*dx + (8*M_PI/8));
    sinusoidalData2D_.value(4, 9) = cos(4*dx + (9*M_PI/8));
    sinusoidalData2D_.value(4, 10) = cos(4*dx + (10*M_PI/8));
    sinusoidalData2D_.value(4, 11) = cos(4*dx + (11*M_PI/8));
    sinusoidalData2D_.value(4, 12) = cos(4*dx + (12*M_PI/8));
    sinusoidalData2D_.value(4, 13) = cos(4*dx + (13*M_PI/8));
    sinusoidalData2D_.value(4, 14) = cos(4*dx + (14*M_PI/8));
    sinusoidalData2D_.value(4, 15) = cos(4*dx + (15*M_PI/8));
    sinusoidalData2D_.value(5, 0) = cos(5*dx + (0*M_PI/8));
    sinusoidalData2D_.value(5, 1) = cos(5*dx + (1*M_PI/8));
    sinusoidalData2D_.value(5, 2) = cos(5*dx + (2*M_PI/8));
    sinusoidalData2D_.value(5, 3) = cos(5*dx + (3*M_PI/8));
    sinusoidalData2D_.value(5, 4) = cos(5*dx + (4*M_PI/8));
    sinusoidalData2D_.value(5, 5) = cos(5*dx + (5*M_PI/8));
    sinusoidalData2D_.value(5, 6) = cos(5*dx + (6*M_PI/8));
    sinusoidalData2D_.value(5, 7) = cos(5*dx + (7*M_PI/8));
    sinusoidalData2D_.value(5, 8) = cos(5*dx + (8*M_PI/8));
    sinusoidalData2D_.value(5, 9) = cos(5*dx + (9*M_PI/8));
    sinusoidalData2D_.value(5, 10) = cos(5*dx + (10*M_PI/8));
    sinusoidalData2D_.value(5, 11) = cos(5*dx + (11*M_PI/8));
    sinusoidalData2D_.value(5, 12) = cos(5*dx + (12*M_PI/8));
    sinusoidalData2D_.value(5, 13) = cos(5*dx + (13*M_PI/8));
    sinusoidalData2D_.value(5, 14) = cos(5*dx + (14*M_PI/8));
    sinusoidalData2D_.value(5, 15) = cos(5*dx + (15*M_PI/8));
    sinusoidalData2D_.value(6, 0) = cos(6*dx + (0*M_PI/8));
    sinusoidalData2D_.value(6, 1) = cos(6*dx + (1*M_PI/8));
    sinusoidalData2D_.value(6, 2) = cos(6*dx + (2*M_PI/8));
    sinusoidalData2D_.value(6, 3) = cos(6*dx + (3*M_PI/8));
    sinusoidalData2D_.value(6, 4) = cos(6*dx + (4*M_PI/8));
    sinusoidalData2D_.value(6, 5) = cos(6*dx + (5*M_PI/8));
    sinusoidalData2D_.value(6, 6) = cos(6*dx + (6*M_PI/8));
    sinusoidalData2D_.value(6, 7) = cos(6*dx + (7*M_PI/8));
    sinusoidalData2D_.value(6, 8) = cos(6*dx + (8*M_PI/8));
    sinusoidalData2D_.value(6, 9) = cos(6*dx + (9*M_PI/8));
    sinusoidalData2D_.value(6, 10) = cos(6*dx + (10*M_PI/8));
    sinusoidalData2D_.value(6, 11) = cos(6*dx + (11*M_PI/8));
    sinusoidalData2D_.value(6, 12) = cos(6*dx + (12*M_PI/8));
    sinusoidalData2D_.value(6, 13) = cos(6*dx + (13*M_PI/8));
    sinusoidalData2D_.value(6, 14) = cos(6*dx + (14*M_PI/8));
    sinusoidalData2D_.value(6, 15) = cos(6*dx + (15*M_PI/8));
    sinusoidalData2D_.value(7, 0) = cos(7*dx + (0*M_PI/8));
    sinusoidalData2D_.value(7, 1) = cos(7*dx + (1*M_PI/8));
    sinusoidalData2D_.value(7, 2) = cos(7*dx + (2*M_PI/8));
    sinusoidalData2D_.value(7, 3) = cos(7*dx + (3*M_PI/8));
    sinusoidalData2D_.value(7, 4) = cos(7*dx + (4*M_PI/8));
    sinusoidalData2D_.value(7, 5) = cos(7*dx + (5*M_PI/8));
    sinusoidalData2D_.value(7, 6) = cos(7*dx + (6*M_PI/8));
    sinusoidalData2D_.value(7, 7) = cos(7*dx + (7*M_PI/8));
    sinusoidalData2D_.value(7, 8) = cos(7*dx + (8*M_PI/8));
    sinusoidalData2D_.value(7, 9) = cos(7*dx + (9*M_PI/8));
    sinusoidalData2D_.value(7, 10) = cos(7*dx + (10*M_PI/8));
    sinusoidalData2D_.value(7, 11) = cos(7*dx + (11*M_PI/8));
    sinusoidalData2D_.value(7, 12) = cos(7*dx + (12*M_PI/8));
    sinusoidalData2D_.value(7, 13) = cos(7*dx + (13*M_PI/8));
    sinusoidalData2D_.value(7, 14) = cos(7*dx + (14*M_PI/8));
    sinusoidalData2D_.value(7, 15) = cos(7*dx + (15*M_PI/8));
    sinusoidalData2D_.value(8, 0) = cos(8*dx + (0*M_PI/8));
    sinusoidalData2D_.value(8, 1) = cos(8*dx + (1*M_PI/8));
    sinusoidalData2D_.value(8, 2) = cos(8*dx + (2*M_PI/8));
    sinusoidalData2D_.value(8, 3) = cos(8*dx + (3*M_PI/8));
    sinusoidalData2D_.value(8, 4) = cos(8*dx + (4*M_PI/8));
    sinusoidalData2D_.value(8, 5) = cos(8*dx + (5*M_PI/8));
    sinusoidalData2D_.value(8, 6) = cos(8*dx + (6*M_PI/8));
    sinusoidalData2D_.value(8, 7) = cos(8*dx + (7*M_PI/8));
    sinusoidalData2D_.value(8, 8) = cos(8*dx + (8*M_PI/8));
    sinusoidalData2D_.value(8, 9) = cos(8*dx + (9*M_PI/8));
    sinusoidalData2D_.value(8, 10) = cos(8*dx + (10*M_PI/8));
    sinusoidalData2D_.value(8, 11) = cos(8*dx + (11*M_PI/8));
    sinusoidalData2D_.value(8, 12) = cos(8*dx + (12*M_PI/8));
    sinusoidalData2D_.value(8, 13) = cos(8*dx + (13*M_PI/8));
    sinusoidalData2D_.value(8, 14) = cos(8*dx + (14*M_PI/8));
    sinusoidalData2D_.value(8, 15) = cos(8*dx + (15*M_PI/8));
    sinusoidalData2D_.value(9, 0) = cos(9*dx + (0*M_PI/8));
    sinusoidalData2D_.value(9, 1) = cos(9*dx + (1*M_PI/8));
    sinusoidalData2D_.value(9, 2) = cos(9*dx + (2*M_PI/8));
    sinusoidalData2D_.value(9, 3) = cos(9*dx + (3*M_PI/8));
    sinusoidalData2D_.value(9, 4) = cos(9*dx + (4*M_PI/8));
    sinusoidalData2D_.value(9, 5) = cos(9*dx + (5*M_PI/8));
    sinusoidalData2D_.value(9, 6) = cos(9*dx + (6*M_PI/8));
    sinusoidalData2D_.value(9, 7) = cos(9*dx + (7*M_PI/8));
    sinusoidalData2D_.value(9, 8) = cos(9*dx + (8*M_PI/8));
    sinusoidalData2D_.value(9, 9) = cos(9*dx + (9*M_PI/8));
    sinusoidalData2D_.value(9, 10) = cos(9*dx + (10*M_PI/8));
    sinusoidalData2D_.value(9, 11) = cos(9*dx + (11*M_PI/8));
    sinusoidalData2D_.value(9, 12) = cos(9*dx + (12*M_PI/8));
    sinusoidalData2D_.value(9, 13) = cos(9*dx + (13*M_PI/8));
    sinusoidalData2D_.value(9, 14) = cos(9*dx + (14*M_PI/8));
    sinusoidalData2D_.value(9, 15) = cos(9*dx + (15*M_PI/8));
    sinusoidalData2D_.value(10, 0) = cos(10*dx + (0*M_PI/8));
    sinusoidalData2D_.value(10, 1) = cos(10*dx + (1*M_PI/8));
    sinusoidalData2D_.value(10, 2) = cos(10*dx + (2*M_PI/8));
    sinusoidalData2D_.value(10, 3) = cos(10*dx + (3*M_PI/8));
    sinusoidalData2D_.value(10, 4) = cos(10*dx + (4*M_PI/8));
    sinusoidalData2D_.value(10, 5) = cos(10*dx + (5*M_PI/8));
    sinusoidalData2D_.value(10, 6) = cos(10*dx + (6*M_PI/8));
    sinusoidalData2D_.value(10, 7) = cos(10*dx + (7*M_PI/8));
    sinusoidalData2D_.value(10, 8) = cos(10*dx + (8*M_PI/8));
    sinusoidalData2D_.value(10, 9) = cos(10*dx + (9*M_PI/8));
    sinusoidalData2D_.value(10, 10) = cos(10*dx + (10*M_PI/8));
    sinusoidalData2D_.value(10, 11) = cos(10*dx + (11*M_PI/8));
    sinusoidalData2D_.value(10, 12) = cos(10*dx + (12*M_PI/8));
    sinusoidalData2D_.value(10, 13) = cos(10*dx + (13*M_PI/8));
    sinusoidalData2D_.value(10, 14) = cos(10*dx + (14*M_PI/8));
    sinusoidalData2D_.value(10, 15) = cos(10*dx + (15*M_PI/8));
    sinusoidalData2D_.value(11, 0) = cos(11*dx + (0*M_PI/8));
    sinusoidalData2D_.value(11, 1) = cos(11*dx + (1*M_PI/8));
    sinusoidalData2D_.value(11, 2) = cos(11*dx + (2*M_PI/8));
    sinusoidalData2D_.value(11, 3) = cos(11*dx + (3*M_PI/8));
    sinusoidalData2D_.value(11, 4) = cos(11*dx + (4*M_PI/8));
    sinusoidalData2D_.value(11, 5) = cos(11*dx + (5*M_PI/8));
    sinusoidalData2D_.value(11, 6) = cos(11*dx + (6*M_PI/8));
    sinusoidalData2D_.value(11, 7) = cos(11*dx + (7*M_PI/8));
    sinusoidalData2D_.value(11, 8) = cos(11*dx + (8*M_PI/8));
    sinusoidalData2D_.value(11, 9) = cos(11*dx + (9*M_PI/8));
    sinusoidalData2D_.value(11, 10) = cos(11*dx + (10*M_PI/8));
    sinusoidalData2D_.value(11, 11) = cos(11*dx + (11*M_PI/8));
    sinusoidalData2D_.value(11, 12) = cos(11*dx + (12*M_PI/8));
    sinusoidalData2D_.value(11, 13) = cos(11*dx + (13*M_PI/8));
    sinusoidalData2D_.value(11, 14) = cos(11*dx + (14*M_PI/8));
    sinusoidalData2D_.value(11, 15) = cos(11*dx + (15*M_PI/8));
    sinusoidalData2D_.value(12, 0) = cos(12*dx + (0*M_PI/8));
    sinusoidalData2D_.value(12, 1) = cos(12*dx + (1*M_PI/8));
    sinusoidalData2D_.value(12, 2) = cos(12*dx + (2*M_PI/8));
    sinusoidalData2D_.value(12, 3) = cos(12*dx + (3*M_PI/8));
    sinusoidalData2D_.value(12, 4) = cos(12*dx + (4*M_PI/8));
    sinusoidalData2D_.value(12, 5) = cos(12*dx + (5*M_PI/8));
    sinusoidalData2D_.value(12, 6) = cos(12*dx + (6*M_PI/8));
    sinusoidalData2D_.value(12, 7) = cos(12*dx + (7*M_PI/8));
    sinusoidalData2D_.value(12, 8) = cos(12*dx + (8*M_PI/8));
    sinusoidalData2D_.value(12, 9) = cos(12*dx + (9*M_PI/8));
    sinusoidalData2D_.value(12, 10) = cos(12*dx + (10*M_PI/8));
    sinusoidalData2D_.value(12, 11) = cos(12*dx + (11*M_PI/8));
    sinusoidalData2D_.value(12, 12) = cos(12*dx + (12*M_PI/8));
    sinusoidalData2D_.value(12, 13) = cos(12*dx + (13*M_PI/8));
    sinusoidalData2D_.value(12, 14) = cos(12*dx + (14*M_PI/8));
    sinusoidalData2D_.value(12, 15) = cos(12*dx + (15*M_PI/8));
    sinusoidalData2D_.value(13, 0) = cos(13*dx + (0*M_PI/8));
    sinusoidalData2D_.value(13, 1) = cos(13*dx + (1*M_PI/8));
    sinusoidalData2D_.value(13, 2) = cos(13*dx + (2*M_PI/8));
    sinusoidalData2D_.value(13, 3) = cos(13*dx + (3*M_PI/8));
    sinusoidalData2D_.value(13, 4) = cos(13*dx + (4*M_PI/8));
    sinusoidalData2D_.value(13, 5) = cos(13*dx + (5*M_PI/8));
    sinusoidalData2D_.value(13, 6) = cos(13*dx + (6*M_PI/8));
    sinusoidalData2D_.value(13, 7) = cos(13*dx + (7*M_PI/8));
    sinusoidalData2D_.value(13, 8) = cos(13*dx + (8*M_PI/8));
    sinusoidalData2D_.value(13, 9) = cos(13*dx + (9*M_PI/8));
    sinusoidalData2D_.value(13, 10) = cos(13*dx + (10*M_PI/8));
    sinusoidalData2D_.value(13, 11) = cos(13*dx + (11*M_PI/8));
    sinusoidalData2D_.value(13, 12) = cos(13*dx + (12*M_PI/8));
    sinusoidalData2D_.value(13, 13) = cos(13*dx + (13*M_PI/8));
    sinusoidalData2D_.value(13, 14) = cos(13*dx + (14*M_PI/8));
    sinusoidalData2D_.value(13, 15) = cos(13*dx + (15*M_PI/8));
    sinusoidalData2D_.value(14, 0) = cos(14*dx + (0*M_PI/8));
    sinusoidalData2D_.value(14, 1) = cos(14*dx + (1*M_PI/8));
    sinusoidalData2D_.value(14, 2) = cos(14*dx + (2*M_PI/8));
    sinusoidalData2D_.value(14, 3) = cos(14*dx + (3*M_PI/8));
    sinusoidalData2D_.value(14, 4) = cos(14*dx + (4*M_PI/8));
    sinusoidalData2D_.value(14, 5) = cos(14*dx + (5*M_PI/8));
    sinusoidalData2D_.value(14, 6) = cos(14*dx + (6*M_PI/8));
    sinusoidalData2D_.value(14, 7) = cos(14*dx + (7*M_PI/8));
    sinusoidalData2D_.value(14, 8) = cos(14*dx + (8*M_PI/8));
    sinusoidalData2D_.value(14, 9) = cos(14*dx + (9*M_PI/8));
    sinusoidalData2D_.value(14, 10) = cos(14*dx + (10*M_PI/8));
    sinusoidalData2D_.value(14, 11) = cos(14*dx + (11*M_PI/8));
    sinusoidalData2D_.value(14, 12) = cos(14*dx + (12*M_PI/8));
    sinusoidalData2D_.value(14, 13) = cos(14*dx + (13*M_PI/8));
    sinusoidalData2D_.value(14, 14) = cos(14*dx + (14*M_PI/8));
    sinusoidalData2D_.value(14, 15) = cos(14*dx + (15*M_PI/8));
    sinusoidalData2D_.value(15, 0) = cos(15*dx + (0*M_PI/8));
    sinusoidalData2D_.value(15, 1) = cos(15*dx + (1*M_PI/8));
    sinusoidalData2D_.value(15, 2) = cos(15*dx + (2*M_PI/8));
    sinusoidalData2D_.value(15, 3) = cos(15*dx + (3*M_PI/8));
    sinusoidalData2D_.value(15, 4) = cos(15*dx + (4*M_PI/8));
    sinusoidalData2D_.value(15, 5) = cos(15*dx + (5*M_PI/8));
    sinusoidalData2D_.value(15, 6) = cos(15*dx + (6*M_PI/8));
    sinusoidalData2D_.value(15, 7) = cos(15*dx + (7*M_PI/8));
    sinusoidalData2D_.value(15, 8) = cos(15*dx + (8*M_PI/8));
    sinusoidalData2D_.value(15, 9) = cos(15*dx + (9*M_PI/8));
    sinusoidalData2D_.value(15, 10) = cos(15*dx + (10*M_PI/8));
    sinusoidalData2D_.value(15, 11) = cos(15*dx + (11*M_PI/8));
    sinusoidalData2D_.value(15, 12) = cos(15*dx + (12*M_PI/8));
    sinusoidalData2D_.value(15, 13) = cos(15*dx + (13*M_PI/8));
    sinusoidalData2D_.value(15, 14) = cos(15*dx + (14*M_PI/8));
    sinusoidalData2D_.value(15, 15) = cos(15*dx + (15*M_PI/8));
}

/*
 * Definition
 */

// Return type of the node
std::string_view TestNode::type() const { return "Test"; }

// Return short summary of the node's purpose
std::string_view TestNode::summary() const { return "A node to allow unit testing of various graph features"; }

// Return the optional Configuration
const std::optional<Configuration> &TestNode::optionalConfiguration() const { return optionalConfiguration_; }

// Return the variant
TestNode::TestVariant TestNode::variant() { return variant_; }

//
void TestNode::emplaceSine(double phi)
{
    // Append sine data
    auto dx = M_PI / 4;
    const auto nPoints = 9;
    for (int i = 0; i < 9; i++)
    {
        auto x = i * dx;
        auto arg = x + phi;
        sineData1D_.xAxis(i) = x;
        sineData1D_.value(i) = sin(arg);
    }
}

//
void TestNode::emplaceCosine(double phi)
{
    // Append cosine data
    auto dx = M_PI / 4;
    const auto nPoints = 9;
    for (int i = 0; i < 9; i++)
    {
        auto x = i * dx;
        auto arg = x + phi;
        cosineData1D_.xAxis(i) = x;
        cosineData1D_.value(i) = cos(arg);
    }
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult TestNode::process()
{
    if (createConfiguration_)
    {
        // Create a Configuration if one doesn't already exist
        if (!optionalConfiguration_)
            optionalConfiguration_.emplace();
    }
    else
        optionalConfiguration_ = std::nullopt;

    // Standard dynamic outputs
    messageParts_.clear();
    messageParts_.insert(messageParts_.end(), message_.begin(), message_.end());

    // Data plotting
    iteration_++;
    emplaceSine(iteration_ * (M_PI / 4) * 0.2);
    emplaceCosine(iteration_ * (M_PI / 4) * 0.2);

    message("Current iteration: {}", iteration_);

    number_ = number_ + 1;

    /*
     * Dynamic outputs
     */

    // Register dynamic (standard) outputs
    for (int i = 0; i < messageParts_.size(); i++)
    {
        auto val = messageParts_[i];
        auto paramName = std::string("Message-Part" + std::format("-{}", i));

        // Check if output already exists - do not add if it does
        if (outputs_.find(paramName) != outputs_.end())
            continue;

        addOutput(paramName, "Part of a message", messageParts_[i]);
    }

    // Register dynamic pointer outputs
    for (int i = 0; i < messageParts_.size(); i++)
    {
        auto val = messageParts_[i];
        auto paramName = std::string("Message-Ptr-Part" + std::format("-{}", i));

        // Check if output already exists - do not add if it does
        if (outputs_.find(paramName) != outputs_.end())
            continue;

        addPointerOutput(paramName, "Part of a message", messageParts_[i]);
    }

    return NodeConstants::ProcessResult::Success;
}
