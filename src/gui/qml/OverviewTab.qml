import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

ColumnLayout {
    id: root
    objectName: "root"
    signal clickedy
    RowLayout {
        spacing: 5
        Repeater {
            model: dissolveModel
            delegate: Rectangle {
                width: 100
                height: 50
                MouseArea {
                    height: parent.height
                    width: parent.width
                    onClicked: emit root.clickedy();
                }
                color: "lightblue"
                border.color: "blue"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(0, columnIndex), Qt.DisplayRole)
                visible: !!content
                Text {
                    anchors.centerIn: parent
                    text: dissolveModel.data(dissolveModel.index(0, columnIndex), Qt.DisplayRole)
                    font.bold: true
                }
            }
        }
    }
    RowLayout {
        spacing: 5
        Repeater {
            model: dissolveModel
            delegate: Rectangle {
                width: 100
                height: 50
                MouseArea {
                    height: parent.height
                    width: parent.width
                    onClicked: {
                        clickedy()
                    }
                }                color: "lightgreen"
                border.color: "green"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(1, columnIndex), Qt.DisplayRole)
                visible: !!content
                Text {
                    anchors.centerIn: parent
                    text: dissolveModel.data(dissolveModel.index(1, columnIndex), Qt.DisplayRole)
                    font.bold: true
                }
            }
        }
    }

    RowLayout {
        spacing: 5
        Repeater {
            model: dissolveModel
            delegate: Rectangle {
                width: 100
                height: 50
                MouseArea {
                    height: parent.height
                    width: parent.width
                    onClicked: {
                        clickedy()
                    }
                }                color: "lightgreen"
                border.color: "green"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(2, columnIndex), Qt.DisplayRole)
                visible: !!content
                Text {
                    anchors.centerIn: parent
                    text: dissolveModel.data(dissolveModel.index(2, columnIndex), Qt.DisplayRole)
                    font.bold: true
                }
            }
        }
    }
}
