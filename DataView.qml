import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12
import QtCharts 2.3
import QtQuick.Layouts 1.12

// balint
//import QtQuick 2.9
//import QtQuick.Controls 2.3
//import QtQuick.Window 2.3
//import QtCharts 2.0
//import QtQuick.Layouts 1.3

//Page {
//    id: pageDV

Rectangle {
    id: container
    property real miny
    property real maxy
    property real minx
    property real maxx
    property real diff
    property alias seriesData: lineSeries
    property alias dataChart: viewDV
    property alias titleY: axisY.titleText
    property alias titleX: axisX.titleText
//    property string lineColor: "green"//: lineSeries.color
//    property var lineStyle//: lineSeries.style



    border.width: 0.5
    radius: 15
    color: root.themeEnum === ChartView.ChartThemeLight ? "white" : Qt.darker( "darkgray" )

    ChartView {
        id: viewDV
        legend.visible: false
        anchors.fill: parent

        antialiasing: true

        animationOptions: ChartView.SeriesAnimations
        animationDuration: 1


        theme: themeEnum

        //        onSeriesAdded: {
        //            console.log("SERIES ADDED SLOT")
        //        }
        //        onSeriesRemoved: {
        //            console.log("SERIES REMOVED SLOT")
        //        }




        ValueAxis {
            id: axisY
            min: miny
            max: maxy
            //onRangeChanged: applyNiceNumbers()
            gridVisible: true
            labelsVisible: true
        }
        ValueAxis {
            id: axisX
            min: minx
            max: maxx
            //            max: if (lineSeries.count == 0)
            //                     minx+cppInterface.maxData // asume f=1 when there is too less data
            //                 else if (lineSeries.count < cppInterface.maxData)
            //                     (minx+((1/root.fs)*cppInterface.maxData))
            //                 else maxx
            gridVisible: false
            labelsVisible: true



        }
        LineSeries {
            id: lineSeries
            axisX: axisX
            axisY: axisY
            pointLabelsVisible: false
            pointLabelsClipping: false
            useOpenGL: true
        }
    }
}
