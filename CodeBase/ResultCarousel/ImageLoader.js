var files = [];
var counter = 0;

function createImage(name, index) {
    innerModel.append({"title": index, "url": name});
}

 function clearCarousel() {
	counter = 0;
	innerModel.clear();
 }