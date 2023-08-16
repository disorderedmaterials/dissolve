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

                    border.color: "steelblue"
                    color: "lightblue"
                    height: 50
                    radius: 5
                    visible: hasContent

                    Component.onCompleted: {
                        itemRect.width = Math.max(100, textItem.width) + 20;
                        textItem.width = itemRect.width;
                    }

                    Text {
                        id: textItem
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        text: hasContent ? dissolveModel.data(dissolveModel.index(rowIndex, columnIndex), Qt.DisplayRole) : ""
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                    }
                    MouseArea {
                        height: parent.height
                        width: parent.width

                        onClicked: {
                            root.nodeClicked(rowIndex, columnIndex);
                        }
                    }
                }
            }
        }
    }
}
