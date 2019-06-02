import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2


//import btmanager 1.0


Dialog {
    id: confDialog
    visible: false
    title: "Custom sensor configuration"
    width: prsMRtext.width + prsMR.width// + 50
    //width: root.width/4
    //height: root.height/2
    standardButtons: Dialog.Ok | Dialog.Cancel

    property alias prs_mr: prsMRslider.value
    property alias prs_osr: prsOSRslider.value
    property alias temp_mr: tmpMRslider.value
    property alias temp_osr: tmpOSRslider.value

    property int prs_mr_prev
    property int prs_osr_prev
    property int temp_mr_prev
    property int temp_osr_prev

    onRejected: {
        prs_mr = prs_mr_prev
        prs_osr = prs_osr_prev
        temp_mr = temp_mr_prev
        temp_osr = temp_osr_prev
    }

    onVisibleChanged: {
        if (visible==true)
        {
            prs_mr_prev = prs_mr
            prs_osr_prev = prs_osr
            temp_mr_prev = temp_mr
            temp_osr_prev = temp_osr
        }
    }

    onAccepted: {
        if (confActions.checkedAction)
            confActions.checkedAction.checked = false

        if (cppInterface.prs_mr != prs_mr)
            cppInterface.prs_mr = prs_mr
        if (cppInterface.prs_osr != prs_osr)
            cppInterface.prs_osr = prs_osr
        if (cppInterface.temp_mr != temp_mr)
            cppInterface.temp_mr = temp_mr
        if (cppInterface.temp_osr != temp_osr)
            cppInterface.temp_osr = temp_osr
        cppInterface.modeState = state_REQUESTED

        // set modes to checked if slider values are the same
        if (prs_mr == 32 && prs_osr == 1 && temp_mr == 32 && temp_osr == 1)
            hdr.checked = true
        if (prs_mr == 8 && prs_osr == 32 && temp_mr == 8 && temp_osr == 32)
            std.checked = true
        if (prs_mr == 1 && prs_osr == 128 && temp_mr == 1 && temp_osr == 128)
            hpr.checked = true
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
        text: qsTr("Temperature Oversampling Rate:  ")
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
