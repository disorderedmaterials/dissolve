import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GroupBox {
    id: root

    property double basey: header.height
    property string image
    property string nodeType
    property int px
    property int py

    signal deleted

    label: RowLayout {
        id: header

        Image {
            clip: true
            fillMode: Image.PreserveAspectFit
            source: image
            sourceSize.height: titleLabel.height
            sourceSize.width: titleLabel.height
        }
        TextField {
            id: titleLabel

            font.pointSize: 14
            text: root.nodeType
        }
        Button {
            contentItem: Image {
                fillMode: Image.PreserveAspectFit
                source: "qrc:/IconsModule/cross.svg"
                sourceSize.height: titleLabel.height
                sourceSize.width: titleLabel.height
            }

            onClicked: root.deleted()
        }
        DragHandler {
            target: null

            xAxis.onActiveValueChanged: delta => posX += delta
            yAxis.onActiveValueChanged: delta => posY += delta
        }
    }
}
