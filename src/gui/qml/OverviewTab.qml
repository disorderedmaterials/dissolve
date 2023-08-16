import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0

ColumnLayout {
    id: root
    signal nodeClicked(int row, int col)

    Repeater {
        model: dissolveModel
        delegate: RowLayout {
            property int rowIndex: index

            Repeater {
                model: dissolveModel.columnCount()
                delegate: Rectangle {
                    id: itemRect
                    property int columnIndex: index
                    property bool hasContent: !!dissolveModel.data(dissolveModel.index(rowIndex, columnIndex), Qt.UserRole)

                    visible: hasContent
                    height: 50
                    color: "lightblue"
                    border.color: "steelblue"
                    radius: 5

                    Text {
                        id: textItem
                        text: hasContent ? dissolveModel.data(dissolveModel.index(rowIndex, columnIndex), Qt.DisplayRole) : ""
                        font.pixelSize: 14
                        wrapMode: Text.WordWrap
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }

                    MouseArea {
                        width: parent.width
                        height: parent.height
                        onClicked: {
                            root.nodeClicked(rowIndex, columnIndex)
                        }
                    }

                    Component.onCompleted: {
                        itemRect.width = Math.max(100, textItem.width) + 20
                        textItem.width = itemRect.width
                    }
                }
            }
        }
    }
}
