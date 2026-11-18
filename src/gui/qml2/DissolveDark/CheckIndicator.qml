// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

import QtQuick

Rectangle {
    id: indicator

    property Item control

    implicitWidth: 16
    implicitHeight: 16

    gradient: Theme.controlBackgroundGradient
    border.color: Theme.colours.mid

    Rectangle {
        x: 1; y: 1
        width: parent.width - 2
        height: parent.width - 2
        border.color: Theme.colours.background
        gradient: Theme.controlBackgroundGradient

        Image {
            x: 1; y: 1
            width: parent.width - 2
            height: parent.width - 2
            visible: indicator.control.checkState === Qt.Checked
            source: "assets/tick.png"
        }

        Rectangle {
            x: 2; y: 2
            width: parent.width - 4
            height: parent.width - 4

            visible: indicator.control.checkState === Qt.PartiallyChecked

            gradient: Theme.getAccentGradient(indicator.control)
            border.color: Theme.colours.background
        }
    }
}
