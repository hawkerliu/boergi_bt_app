// DATAVIEW
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Window 2.3
import QtCharts 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

Page {
    id: pageDV
    //anchors.fill: parent
    width: 640
    height: 480
    property int maxData: 500
    property real miny
    property real maxy
    property real minx
    property real maxx
    property real diff
    property alias seriesData: lineSeries
    property alias dataChart: viewDV

    ChartView {
        id: viewDV
        legend.visible: false
        anchors.fill: parent
        antialiasing: true

//        animationOptions: ChartView.SeriesAnimations
//        animationDuration: 1
//        theme: ChartView.ChartThemeLight
//        //animationOptions: ChartView.NoAnimation
//        //theme: ChartView.ChartThemeDark

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
            //max: maxx
            max: if (lineSeries.count < 3)
                     minx+maxData // asume f=1 when there is too less data
                 else if (lineSeries.count < maxData)
                     (minx+(1/fs)*maxData)
                 else maxx
            gridVisible: false
            labelsVisible: true
        }
        LineSeries {
            id: lineSeries
            axisX: axisX
            axisY: axisY
            pointLabelsVisible: true
            pointLabelsClipping: true
            useOpenGL: true

            onPointAdded: {
                if (lineSeries.count > maxData)
                {
                    minx = lineSeries.at(0).x
                    lineSeries.remove(0)
                }
            }
        }
    }
}
