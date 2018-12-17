function BlinkingSquare(posX, posY, sizeX, sizeY, beats, primaryColour, backgroundColour) {
    this.primaryColour = primaryColour.split(',');
    this.backgroundColour = backgroundColour.split(',');
    this.currentBeat = 0;
    this.beats = beats;
    this.totalBeats = this.beats.length;
    this.posX = posX;
    this.posY = posY;
    this.sizeX = sizeX;
    this.sizeY = sizeY;
    this.lastTime = millis();
    this.drawBeat = true;

    this.display = function () {
        fill(...this.backgroundColour, 10);
        rect(this.posX, this.posY, this.sizeX, this.sizeY);
        if (this.drawBeat) {
            fill(...this.primaryColour);
            rect(this.posX, this.posY, this.sizeX, this.sizeY);
            this.drawBeat = false;
        }
    }

    this.checkBeat = function () {
        if (millis() - this.lastTime > this.beats[this.currentBeat]) {
            this.currentBeat = (this.currentBeat + 1) % this.totalBeats;
            this.lastTime = millis();
            this.drawBeat = true;
        }
    }
}

var squares = new Array();
var squareDims;
var bgColour = "102,177,156";
var primColour = "255,255,255";
function setup() {
    noStroke();
    var canvas = createCanvas(windowWidth, windowHeight);
    canvas.parent('gallery-grid');
    background(...bgColour.split(","));
    squareDims = Math.floor(sqrt(allRhythms.length));
    roundWindowWidth = (Math.ceil(windowWidth / squareDims) * squareDims);
    for (var i = 0; i < (allRhythms.length); i++) {
        var x = (i * squareDims) % (roundWindowWidth);
        var y = Math.floor((i * squareDims) / roundWindowWidth) * squareDims;
        squares[i] = new BlinkingSquare(x, y, squareDims, squareDims, allRhythms[i], primColour, bgColour);
    }
}

function draw() {
    for (var i = 0; i < squares.length; i++) {
        squares[i].checkBeat();
        squares[i].display();
    }
}