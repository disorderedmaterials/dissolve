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
    signal outputSelected(name: string, label: string)
    signal inputSelected(name: string, label: string)

    image: icon
    nodeType: name
    px: posX
    py: posY

    // Ensure that we only display this box for valid node items
    visible: icon != null
    x: posX
    y: posY

    onDeleted: rootModel.deleteNode(index)

    ColumnLayout {
        anchors.fill: parent
        GridLayout {
            columns: 5

            Repeater {
                model: inputs
                Shape {
                    property string title
                    title: name
                    width: 20
                    height: 20
                    Layout.column: 0
                    Layout.row: index
                    Layout.alignment: Qt.AlignLeft
                    ShapePath {
                        fillColor: "black"
                        startX: 20; startY: 0
                        PathLine { x: 20; y: 20 }
                        PathLine { x: 0; y: 10 }
                        PathLine { x: 20; y: 0 }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            inputSelected(root.nodeType, title);
                        }
                    }
                }

                onItemAdded: function (idx, item) {
                    let pos = item.mapToGlobal(item.x, item.y);
                    rootModel.addInput(index, item.title, pos.x, pos.y);
                }

            }
            Repeater {
                model: inputs

                Text {
                    height: 10
                    Layout.column: 1
                    Layout.row: index
                    Layout.alignment: Qt.AlignLeft
                    ToolTip.text: description
                    ToolTip.visible: hovered
                    font.pointSize: 10
                    text: name
                    wrapMode: Text.Wrap
                }
            }

            Repeater {
                model: outputs
                Item {
                    Layout.column: 2
                    Layout.row: index
                    Layout.fillWidth: true
                }
            }


            Repeater {
                model: outputs
                Shape {
                    property string title
                    title: name
                    width: 20
                    height: 20
                    Layout.column: 4
                    Layout.row: index
                    Layout.alignment: Qt.AlignRight
                    ShapePath {
                        fillColor: "black"
                        startX: 0; startY: 0
                        PathLine { x: 0; y: 20 }
                        PathLine { x: 20; y: 10 }
                        PathLine { x: 0; y: 0 }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            outputSelected(root.nodeType, title);
                        }
                    }
                }

                onItemAdded: function (idx, item) {
                    // let pos = Qt.point(item.x, item.y)
                    let pos = item.mapToGlobal(item.x, item.y);
                    pos.x += item.width;
                    rootModel.addOutput(index, item.title, pos.x, pos.y);
                }

            }

            Repeater {
                model: outputs

                Text {
                    Layout.column: 3
                    Layout.row: index
                    Layout.alignment: Qt.AlignRight
                    ToolTip.text: description
                    ToolTip.visible: hovered
                    anchors.margins: 4
                    font.pointSize: 10
                    text: name
                    wrapMode: Text.Wrap
                }
            }
        }
        Rectangle {
            color: palette.active.mid
            height: options.rowCount() > 0 ? 2 : 0
            width: parent.width
        }
        GridLayout {
            columns: 3
            width: parent.width

            Repeater {
                model: options
                Text {
                    text: name
                    Layout.column: 0
                    Layout.row: index
                    Layout.alignment: Qt.AlignLeft
                }
            }

            Repeater {
                model: options
                Item {
                    Layout.column: 1
                    Layout.row: index
                    Layout.fillWidth: true
                }
            }


            Repeater {
                model: options

                delegate: ParameterDelegate {
                }
            }
        }
    }
}
