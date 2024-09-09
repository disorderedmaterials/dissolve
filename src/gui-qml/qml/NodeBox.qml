import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GroupBox {
    id: root
    property string nodeType;
    title: root.nodeType
}
