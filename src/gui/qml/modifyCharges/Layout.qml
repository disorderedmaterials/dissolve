import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../widgets" as D

ColumnLayout {
    property ModifyChargesModel dialogModel
    property int modify
    property int sigFigValue
    property double smoothValue

    anchors.fill: parent
    spacing: 10

    function processSelection(option, x) {
        switch (option) {
        case ModifyChargesModel.Smoothing:
            dialogModel.updateSmoothValue(x);
            break;
        case ModifyChargesModel.ReduceSigFig:
            dialogModel.updateSigFigValue(x);
            break;
        default:
            break;
        }
    }

    D.Text {
        Layout.fillHeight: true
        Layout.fillWidth: true
        text: modify === ModifyChargesModel.Smoothing ? "Enter the target sum to smooth atom charges to" : "Enter the number of significant figures to use for all atoms"
        width: parent.width - 2 * parent.spacing
        wrapMode: Text.WordWrap
    }
    SpinBox {
        id: spinBox
        Layout.alignment: Qt.AlignRight
        Layout.fillWidth: true
        editable: modify === ModifyChargesModel.Smoothing ? true : false
        from: modify === ModifyChargesModel.ReduceSigFig ? 0 : -100
        objectName: modify === ModifyChargesModel.Smoothing ? "smoothSpinBox" : "sigFigSpinBox"
        stepSize: 1
        to: 100
        value: modify === ModifyChargesModel.Smoothing ? dialogModel.smoothValue : dialogModel.sigFigValue
    }
    RowLayout {
        Layout.alignment: Qt.AlignRight
        spacing: 10

        D.Button {
            id: cancelButton
            icon.source: "qrc:/general/icons/false.svg"
            text: "Cancel"

            onClicked: {
                dialogModel.cancelSelection();
                spinBox.value = modify === ModifyChargesModel.Smoothing ? dialogModel.smoothValue : dialogModel.sigFigValue;
            }
        }
        D.Button {
            id: okButton
            icon.source: "qrc:/general/icons/true.svg"
            text: "Ok"

            onClicked: {
                processSelection(modify, spinBox.value);
                dialogModel.acceptSelection();
            }
        }
    }
}
