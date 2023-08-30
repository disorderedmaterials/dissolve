import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property variant model
    property variant modelData

    function generateSpeciesMarkup(index) {
        const speciesData = model.data;
        const speciesIndex = model.index(index, 0);
        const speciesName = speciesData(speciesIndex, Qt.DisplayRole);
        const bonds = speciesData(speciesIndex, Qt.UserRole + 1);
        const angles = speciesData(speciesIndex, Qt.UserRole + 2);
        const torsions = speciesData(speciesIndex, Qt.UserRole + 3);
        const impropers = speciesData(speciesIndex, Qt.UserRole + 4);
        const markup = `
        <div style="font-size: 16px;">
            ${speciesName} <img src="qrc:/general/icons/species.svg" width="15" height="15" alt=""> <br/>
        </div>
        <div style="font-size: 14px; text-align: center;">
            Bonds: ${bonds}<br/>
            Angles: ${angles}<br/>
            Torsions: ${torsions}<br/>
            Impropers: ${impropers}
        </div>
    `;
        return markup;
    }

    Text {
        text: generateSpeciesMarkup(index)
    }
}
