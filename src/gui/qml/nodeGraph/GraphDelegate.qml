import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

NodeBox {
    property variant rootModel
    property double endX: x
    property double midY: y + basey + root.y + root.height / 2 + 10
    property double startX: x + width

    image: icon
    nodeType: name
    px: posX
    py: posY
    x: posX
    y: posY

    onDeleted: rootModel.deleteNode(index)

    Grid {
        columns: 2
        spacing: 2

        Text {
            id: root
            text: "Species"
        }

        Text {
            text: species
        }

        Text {
            text: "Population"
        }

        Text {
            text: population
        }
    }
}
