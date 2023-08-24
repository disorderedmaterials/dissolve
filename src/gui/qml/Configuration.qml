import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

OverviewRectangle {
    property variant model
    property variant modelData

    function generateConfigurationMarkup(index) {
        const configurationData = model.data;
        const configurationIndex = model.index(index, 0);
        const configurationName = configurationData(configurationIndex, Qt.DisplayRole);
        const molecules = configurationData(configurationIndex, Qt.UserRole + 1);
        const atoms = configurationData(configurationIndex, Qt.UserRole + 2);
        const markup = `
        <div style="font-size: 16px;">
            ${configurationName} <img src="qrc:/general/icons/configuration.svg" width="15" height="15"> <br/>
        </div>
        <div style="font-size: 14px; text-align: center;">
            Molecules: ${molecules} <br/>
            Atoms: ${atoms}
        </div>
    `;
        return markup;
    }

    Text {
        text: generateConfigurationMarkup(index)
    }
}
