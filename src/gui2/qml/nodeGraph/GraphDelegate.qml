import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qt.labs.qmlmodels

NodeBox {
    id: root

    property double endX: x
    property double midY: y + height / 2
    property variant rootModel
    property double startX: x + width
    property string currentDescriptionDisplayText: ""

    signal descended(int idx)
    signal edgeCreated(string srcNode, string srcOutput, string tgtNode, string tgtInput)

    image: icon
    nodeType: name
    posX: posX
    posY: posY

    // Ensure that we only display this box for valid node items
    visible: icon != null
    x: posX
    y: posY

    onDeleted: rootModel.deleteNode(index)

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        GridLayout {
            columns: 5

            Repeater {
                id: inputRepeater

                model: inputs

                Component.onCompleted: {
                    for (var i = 0; i < inputRepeater.count; i++) {
                        let item = inputRepeater.itemAt(i);
                        rootModel.addInput(index, item.title, item.x, item.y);
                    }
                }

                Shape {
                    property string nodeName: root.nodeType
                    property string title: name

                    Layout.alignment: Qt.AlignLeft
                    Layout.column: 0
                    Layout.row: index
                    height: 20
                    width: 20

                    ShapePath {
                        fillColor: "black"
                        startX: 20
                        startY: 0

                        PathLine {
                            x: 20
                            y: 20
                        }
                        PathLine {
                            x: 0
                            y: 10
                        }
                        PathLine {
                            x: 20
                            y: 0
                        }
                    }
                    DropArea {
                        anchors.fill: parent

                        onDropped: function (event) {
                            edgeCreated(event.source.parent.nodeName, event.source.parent.title, parent.nodeName, parent.title);
                        }
                    }
                    MouseArea {
                        Drag.active: drag.active
                        Drag.dragType: Drag.Automatic
                        Drag.proposedAction: Qt.LinkAction
                        anchors.fill: parent
                        drag.target: this
                    }
                }
            }
            Repeater {
                model: inputs

                Text {
                    id: inputText
                    property string info: description
                    Layout.alignment: Qt.AlignLeft
                    Layout.column: 1
                    Layout.row: index
                    font.pointSize: 10
                    height: 10
                    text: name
                    wrapMode: Text.Wrap

                    MouseArea {
                        id: inputMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onEntered: {
                            root.currentDescriptionDisplayText = "<i>Input:</i><br>" + inputText.info
                        }

                        onExited: {
                            root.currentDescriptionDisplayText = ""
                        }
                    }
                }
            }
            Repeater {
                model: outputs

                Item {
                    Layout.column: 2
                    Layout.fillWidth: true
                    Layout.row: index
                }
            }
            Repeater {
                id: outputRepeater

                model: outputs

                Component.onCompleted: {
                    for (var i = 0; i < outputRepeater.count; i++) {
                        let item = outputRepeater.itemAt(i);
                        rootModel.addOutput(index, item.title, item.x + item.width / 2, item.y);
                    }
                }

                Shape {
                    property string nodeName: root.nodeType
                    property string title: name

                    Layout.alignment: Qt.AlignRight
                    Layout.column: 4
                    Layout.row: index
                    height: 20
                    width: 20

                    ShapePath {
                        fillColor: "black"
                        startX: 0
                        startY: 0

                        PathLine {
                            x: 0
                            y: 20
                        }
                        PathLine {
                            x: 20
                            y: 10
                        }
                        PathLine {
                            x: 0
                            y: 0
                        }
                    }
                    DropArea {
                        anchors.fill: parent

                        onDropped: function (event) {
                            edgeCreated(parent.nodeName, parent.title, event.source.parent.nodeName, event.source.parent.title);
                        }
                    }
                    MouseArea {
                        Drag.active: drag.active
                        Drag.dragType: Drag.Automatic
                        Drag.proposedAction: Qt.LinkAction
                        anchors.fill: parent
                        drag.target: this
                    }
                }
            }
            Repeater {
                model: outputs

                Text {
                    id: outputText
                    property string info: description
                    Layout.alignment: Qt.AlignRight
                    Layout.column: 3
                    Layout.row: index
                    anchors.margins: 4
                    font.pointSize: 10
                    text: name
                    wrapMode: Text.Wrap

                    MouseArea {
                        id: outputMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onEntered: {
                            root.currentDescriptionDisplayText = "<i>Output:</i><br>" + outputText.info
                        }

                        onExited: {
                            root.currentDescriptionDisplayText = ""
                        }
                    }
                }
            }
        }
        Rectangle {
            color: palette.active.mid
            height: options.rowCount() > 0 ? 2 : 0
            width: parent.width
        }
        Button {
            Layout.fillWidth: true
            text: "Inner Graph"
            visible: inner_graph

            onClicked: {
                descended(index);
            }
        }
        GridLayout {
            columns: 3
            width: parent.width

            Repeater {
                model: options

                Text {
                    id: optionText
                    property string info: description
                    Layout.alignment: Qt.AlignLeft
                    Layout.column: 0
                    Layout.row: index
                    text: name

                    MouseArea {
                        id: optionMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onEntered: {
                            root.currentDescriptionDisplayText = "<i>Option:</i><br>" + optionText.info
                        }

                        onExited: {
                            root.currentDescriptionDisplayText = ""
                        }
                    }
                }
            }
            Repeater {
                model: options

                Item {
                    Layout.column: 1
                    Layout.fillWidth: true
                    Layout.row: index
                }
            }
            Repeater {
                model: options

                delegate: ParameterDelegate {
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true

            visible: root.currentDescriptionDisplayText !== ""

            radius: 4

            color: "cyan"

            border.width: 1
            border.color: "deepskyblue"

            implicitHeight: descriptionText.implicitHeight + 12

            Text {
                id: descriptionText

                anchors.fill: parent
                anchors.margins: 6

                text: root.currentDescriptionDisplayText
                textFormat: Text.RichText
                wrapMode: Text.WordWrap

                font.pixelSize: 11
            }
        }
    }
}
