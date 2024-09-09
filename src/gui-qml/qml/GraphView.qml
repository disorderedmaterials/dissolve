import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

Repeater {
    delegate:
    DelegateChooser {
        role: "type"
        DelegateChoice {
            roleValue: "int"
            delegate: NodeBox {
                x: posX
                y: posY
                nodeType: name
                Text {
                    anchors.fill: parent
                    text: value
                }
            }
        }

        DelegateChoice {
            roleValue: "point2d"
            delegate: NodeBox {
                x: posX
                y: posY
                nodeType: name
                ColumnLayout {
                    anchors.fill: parent
                    Text {text: "X: " + px}
                    Text {text: "Y: " + py}
                }
            }
        }
    }
}
