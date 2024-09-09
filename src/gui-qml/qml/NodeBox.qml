import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GroupBox {
    id: root
    property string nodeType;
    property string image;
    label:
    RowLayout {
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
    }
}
