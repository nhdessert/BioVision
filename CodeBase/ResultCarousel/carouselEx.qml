import QtQuick 1.0
import "ImageLoader.js" as ImageLoader

Rectangle {

	width: 5000; height: 141
    color: "black"

	function displayImage(){
		ImageLoader.createImage();
	}
	
	function passImageInfo(name, index) {
		 ImageLoader.createImage(name, index);
         console.log("Got name and index:", name, " ", index)
         return "success"
		 }
		 
    function clearCarousel() {
		 ImageLoader.clearCarousel();
         return "success"
		 }
		 
	Component.onCompleted: {
        console.log("Model has " + dataModel.count + " images");
        view.currentIndex = 0;
    }

    VisualDataModel {
        id: dataModel
        model: ListModel{
            id:innerModel
        }
		
		delegate: Rectangle {
			id: imageItem
			width: 140;
			height: 125;
			color: "black"
			Text { 
				text: title
				color: "white"
				font.pixelSize: 14
				font.bold: true
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.bottom: parent.bottom }
			Image {
				width: 130;
				height: 100;
				source: url
				fillMode: Image.PreserveAspectFit }
			MouseArea {
                     anchors.fill: parent
					onDoubleClicked: MainWindow.expandCarouselFrame(index, url)
				}
			
		}
	}
	
	PathView {
		 id: view
         anchors.fill: parent
         delegate: delegate
		 model: dataModel
		 preferredHighlightBegin: 0
		 preferredHighlightEnd: 0
		 highlightRangeMode: PathView.StrictlyEnforceRange
		 pathItemCount: 30
         path: Path {
            startX: 100; startY: (view.height)/2
              PathLine { x: view.width ; y: (view.height)/2 }
         }
		 
		
		 
		 Keys.onRightPressed: {
            if (interactive) {
                incrementCurrentIndex()
            }
        }

        Keys.onLeftPressed: {
            if (interactive) {
                decrementCurrentIndex()
            }
        }
		 
	}
	
	
	
}
