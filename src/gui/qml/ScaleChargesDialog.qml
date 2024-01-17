import Dissolve
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "widgets" as D

Page {
    id: root
    property double value

    font.pointSize: 10
    height: 116
    title: "Scale Charges"
    visible: true
    width: 655

    ScaleChargesDialogModel {
        id: dialogModel
        objectName: "dialogModel"
    }

    contentItem: Rectangle {
        height: root.height
        width: root.width

        Column {
            anchors.centerIn: parent
            spacing: 10

            D.Text {
                font.pixelSize: 12
                text: "Enter the scaling factor to apply to all atoms / the target sum to determine scaling factor from"
                wrapMode: Text.Wrap
            }
            SpinBox {
                id: scaleSpinBox
                editable: true
                from: -100
                stepSize: 1
                to: 100
                value: dialogModel.value

                onValueChanged: {
                    dialogModel.updateValue(scaleSpinBox.value);
                }
            }
            Row {
                spacing: 10

                D.Button {
                    id: cancelButton
                    text: "Cancel"

                    onClicked: {
                        dialogModel.cancelSelection();
                    }
                }
                D.Button {
                    id: scaleButton
                    text: "Scale"

                    onClicked: {
                        dialogModel.setOption(dialogModel.Scale);
                        dialogModel.acceptSelection();
                    }
                }
                D.Button {
                    id: scaleToButton
                    text: "Scale To"

                    onClicked: {
                        dialogModel.setOption(dialogModel.ScaleTo);
                        dialogModel.acceptSelection();
                    }
                }
            }
        }
    }
}
