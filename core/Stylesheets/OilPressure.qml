import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QmlStyle 1.0
import "../StartWindow"

CircularGaugeStyle {
    minimumValueAngle: -62
    maximumValueAngle: 60

    property real xCenter: outerRadius
    property real yCenter: outerRadius
    property real needleLength: outerRadius - tickmarkInset * 1.25
    property real needleTipWidth: toPixels(0.02)
    property real needleBaseWidth: toPixels(0.06)
    property color fillStyle : themebox.onCurrentIndexChanged ? themeModel.get(themebox.currentIndex).color : changeColorByTheme(styles.style)

    function toPixels(percentage) {
        return percentage * outerRadius;
    }

    function changeColorByTheme(style){
        if(style === "Material"){
            return Material.foreground
        }else if(style === "Universal"){
            return Universal.foreground
        }
        return "black"
    }

    function needleColor(style){
        var firstColor
        var secondColor

        if(style === "Material"){
            firstColor = Material.accent
            secondColor = Material.primary
            return [firstColor,secondColor]
        }else if(style === "Universal"){
            firstColor = Universal.accent
            secondColor = Universal.primary
            return [firstColor,secondColor]
        }

        firstColor = "red"
        secondColor = "orange"
        return [firstColor, secondColor]
    }
    function degreesToRadians(degrees) {
        return degrees * (Math.PI / 180);
    }

    background: Canvas {
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();

            ctx.beginPath();
            ctx.lineWidth = outerRadius * 0.02;

            ctx.arc(outerRadius, outerRadius, outerRadius - ctx.lineWidth / 2,
                    degreesToRadians(valueToAngle(100) - 90), degreesToRadians(valueToAngle(100) - 90));
        }
    }
    needle: Canvas {
        implicitWidth: needleBaseWidth
        implicitHeight: needleLength

        property real xCenter: width / 2
        property real yCenter: height / 2

        onPaint: {
            var colors = needleColor(styles.style);
            var ctx = getContext("2d");
            ctx.reset();
            ctx.beginPath();
            ctx.moveTo(xCenter, height);
            ctx.lineTo(xCenter - needleBaseWidth / 2, height - needleBaseWidth / 2);
            ctx.lineTo(xCenter - needleTipWidth / 2, 0);
            ctx.lineTo(xCenter, yCenter - needleLength);
            ctx.lineTo(xCenter, 0);
            ctx.closePath();
            ctx.fillStyle = colors[0];
            ctx.fill();

            ctx.beginPath();
            ctx.moveTo(xCenter, height)
            ctx.lineTo(width, height - needleBaseWidth / 2);
            ctx.lineTo(xCenter + needleTipWidth / 2, 0);
            ctx.lineTo(xCenter, 0);
            ctx.closePath();
            ctx.fillStyle = colors[1];
            ctx.fill();
        }
    }

    tickmarkStepSize: 5
    labelStepSize: 5
    tickmark: Rectangle {
        visible: (styleData.value > 0 && styleData.value < 20) || styleData.value % 10 == 0
        implicitWidth: outerRadius * 0.02
        antialiasing: true
        implicitHeight: outerRadius * 0.06
        color: fillStyle

    }

    minorTickmark: Rectangle {
        visible:styleData.value > 0 && styleData.value < 20
        implicitWidth: outerRadius * 0.01
        antialiasing: true
        implicitHeight: outerRadius * 0.03
        color: fillStyle
    }
    tickmarkLabel:  Text {
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        text: styleData.value
        color: fillStyle
        antialiasing: true
    }


}
