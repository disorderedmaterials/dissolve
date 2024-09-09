import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GroupBox {
    id: root
    property string nodeType;
    width: 200
    height: 200
    title: root.nodeType
}
