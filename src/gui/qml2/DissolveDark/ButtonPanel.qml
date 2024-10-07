// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

import QtQuick

Rectangle {
    id: panel

    property Item control
    property bool highlighted: control.highlighted

    visible: !control.flat || control.down || control.checked

    gradient: {
        if (!control.enabled) { Theme.disabledGradient }
        else (control.down || control.checked ? Theme.controlBackgroundGradient : Theme.controlForegroundGradient)
    }

    radius: 2
    border.color: Theme.colours.mid

    Rectangle {
        x: 1; y: 1
        width: parent.width - 2
        height: parent.height - 2
        border.color: Theme.colours.background
        color: "transparent"

        radius: 2
    }
}
