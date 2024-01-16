import Dissolve
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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

            Text {
                font.pixelSize: 12
                text: "Enter the scaling factor to apply to all atoms / the target sum to determine scaling factor from"
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

                Button {
                    id: cancelButton
                    text: "Cancel"

                    onClicked: {
                        dialogModel.cancelSelection();
                    }
                }
                Button {
                    id: scaleButton
                    text: "Scale"

                    onClicked: {
                        dialogModel.setOption(1);
                        dialogModel.acceptSelection();
                    }
                }
                Button {
                    id: scaleToButton
                    text: "Scale To"

                    onClicked: {
                        dialogModel.setOption(2);
                        dialogModel.acceptSelection();
                    }
                }
            }
        }
    }
}
