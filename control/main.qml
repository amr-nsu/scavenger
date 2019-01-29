import QtQuick 2.9
import QtQuick.Window 2.2

Window {
    id: window
    visible: true
    width: 480
    height: 480
    title: qsTr("control")

    Text {
        text: "<p>Up - вперед</p><p>Down - назад</p><p>Left - налево</p><p>Right - направо</p><p>Space - стор</p><p>G - захват</p>"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: 20
        color: "#323232"
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_Up) {
                console.log("f");
            }
            if (event.key === Qt.Key_Down) {
                console.log("b");
            }
            if (event.key === Qt.Key_Left) {
                console.log("l");
            }
            if (event.key === Qt.Key_Right) {
                console.log("r");
            }
            if (event.key === Qt.Key_G) {
                console.log("t");
            }
            if (event.key === Qt.Key_Space) {
                console.log("s");
            }
        }
    }
}
