// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.Button {
    id: control

    property int size: 32

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    width: size
    height: size
    padding: 4
    spacing: 4

    icon.width: size
    icon.height: size

    contentItem: IconLabel {
        width: 50
        height: 50
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        font: Theme.normalFont
        color: Theme.getForegroundColour(control)
    }

    background: ButtonPanel {
        implicitWidth: parent.width
        implicitHeight: parent.height

        control: control
        visible: !control.flat || control.down || control.checked || control.highlighted || control.visualFocus
            || (enabled && control.hovered)
    }
}
