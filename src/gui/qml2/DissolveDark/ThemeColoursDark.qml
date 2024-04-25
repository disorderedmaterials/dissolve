// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick

QtObject {
    // Constants & Factors
    readonly property real brightenFactor: 1.2
    readonly property real darkenFactor: 0.8
    readonly property real extraDarkenFactor: darkenFactor*0.6

    // Core Colour Definitions
    readonly property color foregroundBase: "#ddd"
    readonly property color background: "#101"
    readonly property color principal: "#313"

    // Derived Colours
    readonly property color foregroundNormal: foregroundBase
    readonly property color foregroundHighlighted: brighter(foregroundBase)
    readonly property color foregroundDisabled: extraDark(foregroundBase)
    readonly property color mid: Qt.rgba(
        (background.r + principal.r)*0.5,
        (background.g + principal.g)*0.5,
        (background.b + principal.b)*0.5,
        1.0
    )

    // Colour Functions
    function brighter(colour) {
        return Qt.rgba(colour.r*brightenFactor, colour.g*brightenFactor, colour.b*brightenFactor, 1.0)
    }
    function darker(colour) {
        return Qt.rgba(colour.r*darkenFactor, colour.g*darkenFactor, colour.b*darkenFactor, 1.0)
    }
    function extraDark(colour) {
        return Qt.rgba(colour.r*extraDarkenFactor, colour.g*extraDarkenFactor, colour.b*extraDarkenFactor, 1.0)
    }
}
