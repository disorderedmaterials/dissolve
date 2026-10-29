import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.qmlmodels
import Dissolve
import DissolveIconsModule
import "../DissolveIconsModule"
import "../Dissolve"

DelegateChoice {
    id: root

    function createRangedVector(min, value, max, applyRange) {
        var unRangedValue = [[], [Number(value[0]), Number(value[1]), Number(value[2])], []];
        if (applyRange) {
            var hasMin = !(min[0] == "" && min[1] == "" && min[2] == "");
            var hasMax = !(max[0] == "" && max[1] == "" && max[2] == "");
            if (!(hasMin || hasMax))
                return unRangedValue;
            if (hasMax && !hasMin)
                return [[], [Number(value[0]), Number(value[1]), Number(value[2])], [Number(max[0]), Number(max[1]), Number(max[2])]];
            if (hasMin && !hasMax)
                return [[Number(min[0]), Number(min[1]), Number(min[2])], [Number(value[0]), Number(value[1]), Number(value[2])], []];
            return [[Number(min[0]), Number(min[1]), Number(min[2])], [Number(value[0]), Number(value[1]), Number(value[2])], [Number(max[0]), Number(max[1]), Number(max[2])]];
        }
        return unRangedValue;
    }

    Row {
        Layout.alignment: Qt.AlignRight
        Layout.column: 2
        Layout.row: index
        Layout.fillWidth: true
        spacing: 2

        TextField {
            id: xInput
            text: Number(param[1][0]).toFixed(5)
            width: 30
            validator: DoubleValidator {
                bottom: -10e9
                top: 10e9
                decimals: 5
            }
            onAccepted: param = root.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], applyMinMaxCheckBox.checked)
            onActiveFocusChanged: if (activeFocus)
                selectAll()
            ToolTip.visible: hovered
            ToolTip.text: "X value"
        }
        TextField {
            id: yInput
            text: Number(param[1][1]).toFixed(5)
            width: 30
            validator: DoubleValidator {
                bottom: -10e9
                top: 10e9
                decimals: 5
            }
            onAccepted: param = root.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], applyMinMaxCheckBox.checked)
            onActiveFocusChanged: if (activeFocus)
                selectAll()
            ToolTip.visible: hovered
            ToolTip.text: "Y value"
        }
        TextField {
            id: zInput
            text: Number(param[1][2]).toFixed(5)
            width: 30
            validator: DoubleValidator {
                bottom: -10e9
                top: 10e9
                decimals: 5
            }
            onAccepted: param = root.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], applyMinMaxCheckBox.checked)
            onActiveFocusChanged: if (activeFocus)
                selectAll()
            ToolTip.visible: hovered
            ToolTip.text: "Z value"
        }
        CheckBox {
            id: applyMinMaxCheckBox
            Layout.alignment: Qt.AlignRight
            checked: false
            enabled: root.enabled
            ToolTip.visible: hovered
            ToolTip.text: "Apply min/max"
        }
        Button {
            id: rangeEditButton
            height: applyMinMaxCheckBox.height * 1.2
            width: height
            icon.source: "qrc:/DissolveIconsModule/wizard.svg"
            property Popup editor: Popup {
                id: popup
                width: 120
                height: 180
                parent: Overlay.overlay
                padding: 2
                closePolicy: Popup.NoAutoClose
                property var minRejectAnimation: null
                property var maxRejectAnimation: null
                property var lastMin: ["", "", ""]
                property var lastMax: ["", "", ""]

                function verifyMinRange(min) {
                    var hasMinVector3 = !(min[0] == "" || min[1] == "" || min[2] == "");
                    var isEmpty = min[0] == "" && min[1] == "" && min[2] == "";
                    if (hasMinVector3 || isEmpty)
                        return true;
                    minRejectAnimation.start();
                    return false;
                }
                function verifyMaxRange(max) {
                    var hasMaxVector3 = !(max[0] == "" || max[1] == "" || max[2] == "");
                    var isEmpty = max[0] == "" && max[1] == "" && max[2] == "";
                    if (hasMaxVector3 || isEmpty)
                        return true;
                    maxRejectAnimation.start();
                    return false;
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 2

                    Rectangle {
                        id: header
                        color: "white"
                        border.width: 1
                        border.color: "lightgrey"
                        radius: 3
                        width: parent.width
                        height: 20

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 3
                            anchors.rightMargin: 6
                            spacing: 2

                            Label {
                                Layout.fillWidth: true
                                text: qsTr("Range editor")
                                color: "grey"
                                font.bold: true
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 4

                        GroupBox {
                            id: minGroupBox
                            title: qsTr("Lower bound")
                            Layout.fillWidth: true
                            property color inputColor: applyMinMaxCheckBox.checked ? "transparent" : "lightblue"

                            SequentialAnimation {
                                Component.onCompleted: popup.minRejectAnimation = this
                                PropertyAnimation {
                                    target: minGroupBox
                                    property: "inputColor"
                                    to: "red"
                                    duration: 150
                                }
                                PropertyAnimation {
                                    target: minGroupBox
                                    property: "inputColor"
                                    to: "transparent"
                                    duration: 500
                                }
                            }

                            ColumnLayout {
                                anchors.fill: parent

                                Row {
                                    Layout.fillWidth: true
                                    spacing: 2

                                    TextField {
                                        id: minXInput
                                        background: Rectangle {
                                            color: minGroupBox.inputColor
                                            border.width: 1
                                        }
                                        text: ""
                                        width: 30
                                        enabled: applyMinMaxCheckBox.checked
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            top: 10e9
                                            decimals: 5
                                        }
                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                        ToolTip.visible: hovered
                                        ToolTip.text: "Minimum x"
                                    }
                                    TextField {
                                        id: minYInput
                                        background: Rectangle {
                                            color: minGroupBox.inputColor
                                            border.width: 1
                                        }
                                        text: ""
                                        width: 30
                                        enabled: applyMinMaxCheckBox.checked
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            top: 10e9
                                            decimals: 5
                                        }
                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                        ToolTip.visible: hovered
                                        ToolTip.text: "Minimum y"
                                    }
                                    TextField {
                                        id: minZInput
                                        background: Rectangle {
                                            color: minGroupBox.inputColor
                                            border.width: 1
                                        }
                                        text: ""
                                        width: 30
                                        enabled: applyMinMaxCheckBox.checked
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            top: 10e9
                                            decimals: 5
                                        }
                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                        ToolTip.visible: hovered
                                        ToolTip.text: "Minimum z"
                                    }
                                }
                            }
                        }
                        GroupBox {
                            id: maxGroupBox
                            title: qsTr("Upper bound")
                            Layout.fillWidth: true
                            property color inputColor: applyMinMaxCheckBox.checked ? "transparent" : "lightblue"

                            SequentialAnimation {
                                Component.onCompleted: popup.maxRejectAnimation = this
                                PropertyAnimation {
                                    target: maxGroupBox
                                    property: "inputColor"
                                    to: "red"
                                    duration: 150
                                }
                                PropertyAnimation {
                                    target: maxGroupBox
                                    property: "inputColor"
                                    to: "transparent"
                                    duration: 500
                                }
                            }

                            ColumnLayout {
                                anchors.fill: parent

                                Row {
                                    Layout.fillWidth: true
                                    spacing: 2

                                    TextField {
                                        id: maxXInput
                                        background: Rectangle {
                                            color: maxGroupBox.inputColor
                                            border.width: 1
                                        }
                                        text: ""
                                        width: 30
                                        enabled: applyMinMaxCheckBox.checked
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            top: 10e9
                                            decimals: 5
                                        }
                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                        ToolTip.visible: hovered
                                        ToolTip.text: "Maximum x"
                                    }
                                    TextField {
                                        id: maxYInput
                                        background: Rectangle {
                                            color: maxGroupBox.inputColor
                                            border.width: 1
                                        }
                                        text: ""
                                        width: 30
                                        enabled: applyMinMaxCheckBox.checked
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            top: 10e9
                                            decimals: 5
                                        }
                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                        ToolTip.visible: hovered
                                        ToolTip.text: "Maximum y"
                                    }
                                    TextField {
                                        id: maxZInput
                                        background: Rectangle {
                                            color: maxGroupBox.inputColor
                                            border.width: 1
                                        }
                                        text: ""
                                        width: 30
                                        enabled: applyMinMaxCheckBox.checked
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            top: 10e9
                                            decimals: 5
                                        }
                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                        ToolTip.visible: hovered
                                        ToolTip.text: "Maximum z"
                                    }
                                }
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        anchors.rightMargin: 8
                        spacing: 8

                        Item {
                            Layout.fillWidth: true
                        }

                        Button {
                            text: qsTr("Apply")
                            enabled: applyMinMaxCheckBox.checked
                            onClicked: {
                                const min = [minXInput.text, minYInput.text, minZInput.text];
                                const max = [maxXInput.text, maxYInput.text, maxZInput.text];
                                if (popup.verifyMinRange(min) && popup.verifyMaxRange(max)) {
                                    param = root.createRangedVector(min, [xInput.text, yInput.text, zInput.text], max, applyMinMaxCheckBox.checked);
                                    popup.lastMin = min;
                                    popup.lastMax = max;
                                    popup.close();
                                }
                            }
                        }
                        Button {
                            text: qsTr("Cancel")
                            onClicked: {
                                if (!popup.verifyMinRange([minXInput.text, minYInput.text, minZInput.text])) {
                                    minXInput.text = popup.lastMin[0];
                                    minYInput.text = popup.lastMin[1];
                                    minZInput.text = popup.lastMin[2];
                                }
                                if (!popup.verifyMaxRange([maxXInput.text, maxYInput.text, maxZInput.text])) {
                                    maxXInput.text = popup.lastMax[0];
                                    maxYInput.text = popup.lastMax[1];
                                    maxZInput.text = popup.lastMax[2];
                                }
                                popup.close();
                            }
                        }
                    }
                }
            }
            onClicked: {
                const p = rangeEditButton.mapToItem(Overlay.overlay, rangeEditButton.width, 0);
                editor.x = p.x;
                editor.y = p.y;
                editor.open();
            }
            ToolTip.visible: hovered
            ToolTip.text: "Edit x, y, and z ranges"
        }
    }
}
