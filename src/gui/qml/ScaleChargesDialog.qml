import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
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

    Item {
        anchors.fill: parent
        anchors.margins: 10

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            D.Text {
                width: parent.width-2*parent.spacing
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
                Layout.alignment: Qt.AlignRight
            }
            RowLayout {
                spacing: 10
                Layout.alignment: Qt.AlignRight

                D.Button {
                    id: scaleButton
                    icon.source: "qrc:/general/icons/true.svg"
                    text: "Scale"                    

                    onClicked: {
                        dialogModel.processSelection(dialogModel.Scale, scaleSpinBox.value);
                        dialogModel.acceptSelection();
                    }
                }
                D.Button {
                    id: scaleToButton
                    icon.source: "qrc:/general/icons/true.svg"
                    text: "Scale To"

                    onClicked: {
                        dialogModel.processSelection(dialogModel.ScaleTo, scaleSpinBox.value);
                        dialogModel.acceptSelection();
                    }
                }
                D.Button {
                    id: cancelButton
                    icon.source: "qrc:/general/icons/false.svg"
                    text: "Cancel"

                    onClicked: {
                        dialogModel.cancelSelection();
                        scaleSpinBox.value = dialogModel.value;
                    }
                }
            }
        }
    }
}
