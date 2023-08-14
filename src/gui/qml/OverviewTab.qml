import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Rectangle {
    signal clickedy
    width: 100
    height: 50
    color: "lightblue"
    border.color: "blue"
    border.width: 2
    TapHandler {
        onTapped: {
            clickedy();
            console.log("oof");
        }
    }
}

/*
ColumnLayout {
    id: root
    signal clickedy()
*/

    /*RowLayout {
        spacing: 5
        Repeater {
            model: 5
            delegate: Rectangle {
                width: 100
                height: 50
                color: "lightblue"
                border.color: "blue"
                border.width: 2

                property int columnIndex: index

                TapHandler {
                    onTapped: {
                        root.clickedy();
                    }
                }
            }
        }*/
/*
ColumnLayout {
    id: root
    signal clickedy(string text)
    property string propagateText: ""

    onPropagateTextChanged: {
        clickedy(propagateText);
    }

    RowLayout {
        spacing: 5
        Repeater {
            model: dissolveModel
            delegate: Rectangle {
                width: 100
                height: 50
                color: "lightblue"
                border.color: "blue"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(0, columnIndex), Qt.DisplayRole)
                //visible: !!content
                Text {
                    anchors.centerIn: parent
                    text: dissolveModel.data(dissolveModel.index(0, columnIndex), Qt.DisplayRole)
                    font.bold: true
                }
                TapHandler {
                    onTapped: {
                        root.propagateText = "Test!";
                        console.log("Test");
                    }
                }
            }
        }
    }
}*/
    /*
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
                    onClicked: root.clickedy()
                }
                color: "lightgreen"
                border.color: "green"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(1, columnIndex), Qt.DisplayRole)
                //visible: !!content
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
                    onClicked: root.clickedy()
                }
                color: "lightgreen"
                border.color: "green"
                border.width: 2
                property int columnIndex: index
                property string content: dissolveModel.data(dissolveModel.index(2, columnIndex), Qt.DisplayRole)
                //visible: !!content
                Text {
                    anchors.centerIn: parent
                    text: dissolveModel.data(dissolveModel.index(2, columnIndex), Qt.DisplayRole)
                    font.bold: true
                }
            }
        }
    }*/