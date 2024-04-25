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

    // Choose theme colours (TODO Light / normal version)
    property ThemeColoursDark colours: ThemeColoursDark {}

    function getForegroundColour(control){
            if (!control.enabled) { return colours.foregroundDisabled }
            else return (control.down || control.checked ? colours.foregroundHighlighted : colours.foregroundNormal)
        }

    function getForegroundGradient(control){
        if (!control.enabled) { return foregroundDisabledGradient }
        else return (control.down || control.checked ? Theme.foregroundHighlightGradient : Theme.foregroundNormalGradient)
    }

    // Gradients
    property Gradient mainGradient: Gradient {
        GradientStop { position: 0.0; color: colours.background }
        GradientStop { position: 0.9; color: colours.background }
        GradientStop { position: 1.0; color: colours.principal }
    }
    property Gradient foregroundNormalGradient: Gradient {
        GradientStop { position: 0.0; color: colours.foregroundNormal }
        GradientStop { position: 1.0; color: colours.foregroundDisabled }
    }
    property Gradient foregroundHighlightGradient: Gradient {
        GradientStop { position: 0.0; color: colours.foregroundNormal }
        GradientStop { position: 1.0; color: colours.foregroundHighlighted }
    }
    property Gradient foregroundDisabledGradient: Gradient {
        GradientStop { position: 0.0; color: colours.foregroundDisabled }
        GradientStop { position: 1.0; color: colours.background }
    }
    property Gradient disabledGradient: Gradient {
        GradientStop { position: 0.0; color: colours.extraDark(colours.principal) }
        GradientStop { position: 1.0; color: colours.background }
    }
    property Gradient controlForegroundGradient: Gradient {
        GradientStop { position: 0.0; color: colours.brighter(colours.principal) }
        GradientStop { position: 1.0; color: colours.darker(colours.principal) }
    }
    property Gradient controlBackgroundGradient: Gradient {
        GradientStop { position: 0.0; color: colours.darker(colours.principal) }
        GradientStop { position: 1.0; color: colours.extraDark(colours.principal) }
    }
    property Gradient testGradient: Gradient {
        GradientStop { position: 0.0; color: "#100" }
        GradientStop { position: 1.0; color: "#110" }
    }

}
