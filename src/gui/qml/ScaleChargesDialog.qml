import Dissolve
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "widgets" as D

Page {
    id: root
    property double value

    height: 116
    title: "Scale Charges"
    visible: true
    width: 655

    ScaleChargesDialogModel {
        id: dialogModel
        objectName: "dialogModel"

        function processSelection(mode, x) {
            dialogModel.setOption(mode);
            dialogModel.updateValue(x);
        }
    }

    contentItem: Rectangle {
        height: root.height
        width: root.width

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 10

            D.Text {
                font.pixelSize: 11
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
            }
            RowLayout {
                spacing: 10

                D.Button {
                    id: cancelButton
                    text: "Cancel"

                    onClicked: {
                        dialogModel.cancelSelection();
                        scaleSpinBox.value = dialogModel.value;
                    }
                }
                D.Button {
                    id: scaleButton
                    text: "Scale"

                    onClicked: {
                        dialogModel.processSelection(dialogModel.Scale, scaleSpinBox.value);
                        dialogModel.acceptSelection();
                    }
                }
                D.Button {
                    id: scaleToButton
                    text: "Scale To"

                    onClicked: {
                        dialogModel.processSelection(dialogModel.ScaleTo, scaleSpinBox.value);
                        dialogModel.acceptSelection();
                    }
                }
            }
        }
    }
}
