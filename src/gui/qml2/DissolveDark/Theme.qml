// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

pragma Singleton

import QtQuick

QtObject {
    id: root

    property real baseSize: 1.0

    property font normalFont: Qt.font({
        pointSize: 10 * baseSize
    })

    // Core Colour Definitions
    // -- Constants & Factors
    property real brightenFactor: 1.2
    property real darkenFactor: 0.8
    readonly property real extraDarkenFactor: darkenFactor*darkenFactor
    // -- Dark
    property color foreground: "#ddd"
    property color background: "#101"
    property color principal: "#313"

    readonly property color normalText: foreground
    readonly property color highlightedText: Qt.rgba(foreground.r*brightenFactor, foreground.g*brightenFactor, foreground.b*brightenFactor, 1.0)
    readonly property color disabledText: Qt.rgba(foreground.r*extraDarkenFactor, foreground.g*extraDarkenFactor, foreground.b*extraDarkenFactor, 1.0)
    readonly property color mid: Qt.rgba(
        (background.r + principal.r)*0.5,
        (background.g + principal.g)*0.5,
        (background.b + principal.b)*0.5,
        1.0
    )

    // Gradients
    property Gradient mainGradient: Gradient {
        GradientStop { position: 0.0; color: background }
        GradientStop { position: 0.9; color: background }
        GradientStop { position: 1.0; color: principal }
    }
    property Gradient disabledGradient: Gradient {
        GradientStop { position: 0.0; color: Qt.rgba(principal.r*extraDarkenFactor, principal.g*extraDarkenFactor, principal.b*extraDarkenFactor, 1.0) }
        GradientStop { position: 1.0; color: background }
    }
    property Gradient buttonUpGradient: Gradient {
        GradientStop { position: 0.0; color: Qt.rgba(principal.r*brightenFactor, principal.g*brightenFactor, principal.b*brightenFactor, 1.0) }
        GradientStop { position: 1.0; color: Qt.rgba(principal.r*darkenFactor, principal.g*darkenFactor, principal.b*darkenFactor, 1.0) }
    }
    property Gradient buttonDownGradient: Gradient {
        GradientStop { position: 0.0; color: Qt.rgba(principal.r*darkenFactor, principal.g*darkenFactor, principal.b*darkenFactor, 1.0) }
        GradientStop { position: 1.0; color: Qt.rgba(principal.r*extraDarkenFactor, principal.g*extraDarkenFactor, principal.b*extraDarkenFactor, 1.0) }
    }
}
