import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0
import "../widgets" as D

Page {
    id: root
    height: 500
    visible: true
    width: 670

    function filterByRegExp(proxy, text) {
        proxy.filterRegularExpression = RegExp(text);
    }
    function getHeaderStringArray(model) {
        var headerArray = [];
        for (var i = 0; i < model.columnCount(); ++i) {
            headerArray.push(qsTr(model.headerData(i, Qt.Horizontal)));
        }
        return headerArray;
    }

    GroupBox {
        id: gb
        anchors.fill: parent
        title: "Current Module Data"

        ColumnLayout {
            anchors.fill: parent

            TextField {
                id: searchBox
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: gb.width / 4
                placeholderText: qsTr("Search...")

                onEditingFinished: {
                    filterByRegExp(simProxy, searchBox.text);
                    if (simProxy.rowCount() == 0) {
                        filterByRegExp(simProxy, "");
                    }
                }
            }
            HorizontalHeaderView {
                id: header
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                Layout.preferredWidth: contentWidth
                clip: true
                enabled: simModel.rowCount() == 0 ? false : true
                model: getHeaderStringArray(simModel)
                syncView: table
            }
            TableView {
                id: table
                property variant colWidths: [300, 300, 50]

                Layout.fillHeight: true
                Layout.fillWidth: true
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                columnSpacing: 1
                columnWidthProvider: function (column) {
                    return colWidths[column];
                }
                model: simProxy
                rowSpacing: 1

                delegate: Rectangle {
                    color: "white"
                    implicitHeight: tableText.height
                    implicitWidth: tableText.width

                    D.Text {
                        id: tableText
                        padding: 12
                        text: display
                    }
                }
            }
            Button {
                id: closeButton
                Layout.alignment: Qt.AlignRight
                text: "Close"

                onClicked: simModel.closeClicked()
            }
        }
    }
}
