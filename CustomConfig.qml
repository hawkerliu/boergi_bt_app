import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2


//import btmanager 1.0


Dialog {
    id: confDialog
    visible: true
    title: "Custom sensor configuration"
    width: prsMRtext.width + prsMR.width// + 50
    //width: root.width/4
    //height: root.height/2
    standardButtons: Dialog.Ok | Dialog.Cancel | Dialog.Reset

    signal testSignal() // ********************************************************************

    onAccepted: {
        console.log("Ok clicked")
        confActions.checkedAction.checked = false
        console.log("Apply settings to setMode() here!")
    }
    onRejected: {
        console.log("Cancel clicked")
        confDialog.testSignal() // ****************************************************************
    }
    onReset: {
        console.log("Reset clicked")
        console.log("Reset settings to standard here!")
    }

    // PRS MR
    Text {
        id: prsMRtext
        text: qsTr("Pressure Measurement Rate:  ")
        font.pixelSize: 20
        anchors.top: parent.top
    }
    Text {
        id: prsMR
        text: prsMRslider.value
        anchors.left: prsMRtext.right
        height: prsMRtext.height + 10
        width: 49
        font.pixelSize: prsMRtext.font.pixelSize
        font.bold: true
    }
    ConfigSlider {
    id: prsMRslider
    anchors.top: prsMR.bottom
    }

    // PRS OSR
    Text {
        id: prsOSRtext
        text: qsTr("Pressure Oversampling Rate:  ")
        font.pixelSize: 20
        anchors.top: prsMRslider.bottom
    }
    Text {
        id: prsOSR
        text: prsOSRslider.value
        anchors.left: prsOSRtext.right
        anchors.top: prsOSRtext.top
        height: prsOSRtext.height + 10
        width: 49
        font.pixelSize: prsOSRtext.font.pixelSize
        font.bold: true
    }
    ConfigSlider {
    id: prsOSRslider
    anchors.top: prsOSR.bottom
    }

    // TMP MR
    Text {
        id: tmpMRtext
        text: qsTr("Temperature Measurement Rate:  ")
        font.pixelSize: 20
        anchors.top: prsOSRslider.bottom
    }
    Text {
        id: tmpMR
        text: tmpMRslider.value
        anchors.left: tmpMRtext.right
        anchors.top: tmpMRtext.top
        height: tmpMRtext.height + 10
        width: 49
        font.pixelSize: tmpMRtext.font.pixelSize
        font.bold: true
    }
    ConfigSlider {
    id: tmpMRslider
    anchors.top: tmpMR.bottom
    }

    // TMP OSR
    Text {
        id: tmpOSRtext
        text: qsTr("Temperature Measurement Rate:  ")
        font.pixelSize: 20
        anchors.top: tmpMRslider.bottom
    }
    Text {
        id: tmpOSR
        text: tmpOSRslider.value
        anchors.left: tmpOSRtext.right
        anchors.top: tmpOSRtext.top
        height: tmpOSRtext.height + 10
        width: 49
        font.pixelSize: tmpOSRtext.font.pixelSize
        font.bold: true
    }
    ConfigSlider {
    id: tmpOSRslider
    anchors.top: tmpOSR.bottom
    }





}
