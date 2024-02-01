import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "widgets" as D

Page {
    id: root

    property double scaleValue
    property double smoothValue
    property double sigFigValue

    height: 116
    title: {
            switch(modify) {
                case ModifyChargesModel.Scaling:
                    return "Scale Charges";
                case ModifyChargesModel.Smoothing:
                    return "Smooth Charges";
                case ModifyChargesModel.ReduceSigFig:
                    return "Reduce Significant Figures";
                default:
                    break;
            }
        }
    visible: true
    width: 400

    Component {
        id: scaleButtons
        
        Item {
            D.Button {
                id: cancelButton
                icon.source: "qrc:/general/icons/false.svg"
                text: "Cancel"

                onClicked: {
                    dialogModel.cancelSelection();
                    spinBox.value = getModifierValue(modify)
                }
            }
            D.Button {
                id: scaleButton
                icon.source: "qrc:/general/icons/true.svg"
                text: "Scale"

                onClicked: {
                    processSelection(modify, ModifyChargesModel.Scale, spinBox.value);
                    dialogModel.acceptSelection();
                }
            }
            D.Button {
                id: scaleToButton
                icon.source: "qrc:/general/icons/true.svg"
                text: "Scale To"

                onClicked: {
                    processSelection(modify, ModifyChargesModel.ScaleTo, spinBox.value);
                    dialogModel.acceptSelection();
                }
            }
        }
    }
    Component {
        id: buttons

        Item {
            D.Button {
                id: cancelButton
                icon.source: "qrc:/general/icons/false.svg"
                text: "Cancel"

                onClicked: {
                    dialogModel.cancelSelection();
                    spinBox.value = getModifierValue(modify)
                }
            }
            D.Button {
                id: scaleButton
                icon.source: "qrc:/general/icons/true.svg"
                text: "Ok"

                onClicked: {
                    processSelection(modify, null, spinBox.value);
                    dialogModel.acceptSelection();
                }
            }
        }
    }

    function processSelection(option, scaleType, x) {
        switch(option) {
            case ModifyChargesModel.Scaling:
                dialogModel.setScaleType(scaleType);
                dialogModel.updateScaleValue(x);
                break;
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

    function getModifierValue(option) {
        switch(option) {
            case ModifyChargesModel.Scaling:
                return dialogModel.scaleValue;
            case ModifyChargesModel.Smoothing:
                return dialogModel.smoothValue;
            case ModifyChargesModel.ReduceSigFig:
                return dialogModel.sigFigValue;
            default:
                break;
        }
    }
    Item {
        anchors.fill: parent
        anchors.margins: 10

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            D.Text {
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pixelSize: 11
                text: {
                    switch(modify) {
                        case ModifyChargesModel.Scaling:
                            return "Enter the scaling factor to apply to all atoms / the target sum to determine scaling factor from";
                        case ModifyChargesModel.Smoothing:
                            return "Enter the target sum to smooth atom charges to";
                        case ModifyChargesModel.ReduceSigFig:
                            return "Enter the number of significant figures to use for all atoms";
                        default:
                            break
                    }
                }
                
                width: parent.width - 2 * parent.spacing
                wrapMode: Text.WordWrap
            }
            SpinBox {
                id: spinBox
                objectName: "spinBox"
                Layout.alignment: Qt.AlignRight
                Layout.fillWidth: true
                editable: modify === ModifyChargesModel.ReduceSigFig ? false : true
                from: modify === ModifyChargesModel.ReduceSigFig ? 0 : -100
                stepSize: 1
                to: 100
                value: getModifierValue(modify)
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: 10
                
                Loader { 
                    sourceComponent: modify === ModifyChargesModel.Scaling ? buttons : scaleButtons
                }
            }
        }
    }
}
