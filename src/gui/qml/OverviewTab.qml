import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0
import Dissolve 1.0

ColumnLayout {
    id: root
    signal clicked(int row, int col)
    Repeater {
        model: dissolveModel
        delegate: RowLayout {
            property int rowIndex: index
            Repeater {
                model: dissolveModel
                delegate: Rectangle {
                    property int columnIndex: index
                    property bool hasContent: !!dissolveModel.data(dissolveModel.index(rowIndex, columnIndex), Qt.UserRole)
                    visible: hasContent
                    width: 100
                    height: 50
                    color: "lightblue"
                    border.color: "steelblue"
                    radius: 5

                    Text {
                        text: hasContent ? dissolveModel.data(dissolveModel.index(rowIndex, columnIndex), Qt.DisplayRole) : ""
                        width: parent.width
                        font.pixelSize: 14
                        wrapMode: Text.WordWrap
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }


                    MouseArea {
                        width: parent.width
                        height: parent.height
                        onClicked: {
                            root.clicked(rowIndex, columnIndex)
                        }
                    }

                }
            }
        }
    }
}