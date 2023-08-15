import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0

ColumnLayout {
    id: root
    signal clicked(int row, int col)
    spacing: 20
    RowLayout {
        spacing: 20
        Repeater {
            model: dissolveModel ? dissolveModel : 0
            delegate: Rectangle {
                width: 100
                height: 50
                color: "lightblue"
                border.color: "blue"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(0, columnIndex), Qt.DisplayRole) ? dissolveModel.data(dissolveModel.index(0, columnIndex), Qt.DisplayRole) : ""
                visible: !!content
                Text {
                    anchors.centerIn: parent
                    text: content
                    font.bold: true
                }
                MouseArea {
                    width: 100
                    height: 50
                    onClicked: {
                        root.clicked(0, columnIndex)
                    }
                }
            }
        }
    }
    RowLayout {
        spacing: 20
        Repeater {
            model: dissolveModel
            delegate: Rectangle {
                width: 100
                height: 50
                color: "lightgreen"
                border.color: "green"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(1, columnIndex), Qt.DisplayRole) ? dissolveModel.data(dissolveModel.index(1, columnIndex), Qt.DisplayRole) : ""
                visible: !!content
                Text {
                    anchors.centerIn: parent
                    text: content
                    font.bold: true
                }
                MouseArea {
                    width: 100
                    height: 50
                    onClicked: {
                        root.clicked(1, columnIndex)
                    }
                }
            }
        }
    }
    RowLayout {
        spacing: 20
        Repeater {
            model: dissolveModel
            delegate: Rectangle {
                width: 100
                height: 50
                color: "lightgreen"
                border.color: "green"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(2, columnIndex), Qt.DisplayRole) ? dissolveModel.data(dissolveModel.index(2, columnIndex), Qt.DisplayRole) : ""
                visible: !!content
                Text {
                    anchors.centerIn: parent
                    text: content
                    font.bold: true
                }
                MouseArea {
                    width: 100
                    height: 50
                    onClicked: {
                        root.clicked(2, columnIndex)
                    }
                }
            }
        }
    }
}
