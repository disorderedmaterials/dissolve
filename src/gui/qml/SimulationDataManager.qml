import QtQuick 2.12
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "widgets" as D

GroupBox {
    id: root
    height: 166
    title: "Current Module Data"
    visible: true
    width: 400
    
    ColumnLayout {
        TableView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columnSpacing: 1
            rowSpacing: 1
            clip: true

            model: root.simModel

            delegate: Rectangle {
                implicitWidth: 100
                implicitHeight: 50
                Text {
                    text: display
                }
            }
        }

        D.Button {
            text: "Close"
/*             onClicked: {
                model.cancelSelection()
            } */
        }
    }
}
