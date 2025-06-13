import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

NodeBox {
    id: root

    property double endX: x
    property double midY: y + height / 2
    property variant rootModel
    property double startX: x + width

    image: icon
    nodeType: name
    px: posX
    py: posY

    // Ensure that we only display this box for valid node items
    visible: icon != null
    x: posX
    y: posY

    onDeleted: rootModel.deleteNode(index)

    Column {
        Row {
            Column {
                Repeater {
                    model: inputs

                    delegate: Text {
                        ToolTip.text: description
                        ToolTip.visible: hovered
                        anchors.margins: 4
                        font.pointSize: 10
                        text: name
                        wrapMode: Text.Wrap
                    }

                    onItemAdded: function (idx, item) {
                        let pos = item.mapToGlobal(item.x, item.y);
                        pos.x -= 2 * item.anchors.margins;
                        pos.y += item.height / 2;
                        pos.y += 35; // Adjust for title
                        console.log("Input!", item.text, item.width, item.height);
                        console.log(idx);
                        console.log(pos);
                        rootModel.addInput(index, item.text, pos.x, pos.y);
                    }
                }
            }
            Rectangle {
                color: palette.active.mid
                height: parent.height
                width: (inputs.rowCount() > 0 && outputs.rowCount() > 0) ? 2 : 0
            }
            Column {
                Repeater {
                    model: outputs

                    delegate: Text {
                        ToolTip.text: description
                        ToolTip.visible: hovered
                        anchors.margins: 4
                        font.pointSize: 10
                        text: name
                        wrapMode: Text.Wrap
                    }

                    onItemAdded: function (idx, item) {
                        // let pos = Qt.point(item.x, item.y)
                        let pos = item.mapToGlobal(item.x, item.y);
                        pos.x += 2 * item.anchors.margins;
                        pos.x += item.width;
                        pos.y += item.height;
                        pos.y += 35; // Adjust for title
                        console.log("Output!", item.text, item.width, item.height);
                        console.log(idx);
                        console.log(pos);
                        rootModel.addOutput(index, item.text, pos.x, pos.y);
                    }
                }
            }
        }
        Rectangle {
            color: palette.active.mid
            height: options.rowCount() > 0 ? 2 : 0
            width: parent.width
        }
        Column {
            Repeater {
                model: options

                delegate: ParameterDelegate {
                }
            }
        }
    }
}
