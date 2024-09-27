import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GroupBox {
    id: root
    property string nodeType;
    property string image;
    property double basey;
    property int px;
    property int py;
    signal deleted;
    basey: header.height
    label:
    RowLayout {
        id: header
        Image {
            clip: true
            sourceSize.width: titleLabel.height
            sourceSize.height: titleLabel.height
            fillMode: Image.PreserveAspectFit
            source: image
        }
        Text {
            id: titleLabel
            text: root.nodeType
            font.pointSize: 14
        }
        Button {
            contentItem: Image {
                sourceSize.width: titleLabel.height
                sourceSize.height: titleLabel.height
                fillMode: Image.PreserveAspectFit
                source: "qrc:/Dissolve/icons/cross.svg"
            }
            onClicked: root.deleted()
        }

        DragHandler {
            target: null
            xAxis.onActiveValueChanged: (delta) => posX += delta
            yAxis.onActiveValueChanged: (delta) => posY += delta
        }
    }
}
