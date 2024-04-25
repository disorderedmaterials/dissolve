// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Controls.Fusion
import QtQuick.Controls.Fusion.impl

Rectangle {
    id: indicator

    property Item control
    readonly property color pressedColor: Fusion.mergedColors(control.palette.base, control.palette.windowText, 85)
    readonly property color checkMarkColor: Qt.darker(control.palette.text, 1.2)

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
