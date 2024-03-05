import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0
import "../widgets" as D

Page {
    id: root
    width: 680
    height: 400
    visible: true

    GroupBox {
        id: gb
        title: "Current Module Data"
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent

            TextField {
                id: searchBox
                Layout.preferredWidth: gb.width/4
                Layout.alignment: Qt.AlignRight
                placeholderText: qsTr("Search...")
                onEditingFinished: {
                    filterByRegExp(simProxy, searchBox.text)
                    if (simProxy.rowCount() == 0) {
                        filterByRegExp(simProxy, "")
                    }
                }
            }

            HorizontalHeaderView {
                id: header

                enabled: simModel.rowCount() == 0 ? false : true 

                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                Layout.preferredWidth: contentWidth

                syncView: table
                model: getHeaderStringArray(simModel)
                clip: true
            }

            TableView {
                id: table   

                Layout.fillWidth: true
                Layout.fillHeight: true

                columnSpacing: 1
                rowSpacing: 1
                boundsBehavior: Flickable.StopAtBounds

                model: simProxy

                delegate:
                Rectangle {
                    implicitWidth: tableText.width
                    implicitHeight: tableText.height
                    color: "white"


                    D.Text {
                        id: tableText
                        text: display

                        padding: 12
                    }
                }
            }

            Button {
                text: "Close"
                Layout.alignment: Qt.AlignRight
                onClicked: simModel.closeClicked()
            }
        }
    }

    function getHeaderStringArray(model) {
        var headerArray = [];
        for (var i=0; i < model.columnCount(); ++i ) {
            headerArray.push(qsTr(model.headerData(i, Qt.Horizontal)));
        }
        return headerArray
    }

    function filterByRegExp(proxy, text) {
        proxy.filterRegularExpression = RegExp(text);
    }
}
