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
    var canvas = createCanvas(800, 800);
    canvas.parent('gallery-grid');
    background(...bgColour.split(","));
    squaresPer = 32;
    squareDims = width / squaresPer;
    for (var i = 0; i < (allRhythms.length); i++) {
        var x = (i * squareDims) % width;
        var y = Math.floor((i * squareDims) / height) * squareDims;
        squares[i] = new BlinkingSquare(x, y, squareDims, squareDims, allRhythms[i], primColour, bgColour);
    }
    /*
    for (var i = 0; i < (squaresPer * squaresPer); i++) {
        var x = (i * squareDims) % width;
        var y = Math.floor((i * squareDims) / height) * squareDims;
        squares[i] = new BlinkingSquare(x, y, squareDims, squareDims, [parseInt(Math.random() * 2000), parseInt(Math.random() * 1000), parseInt(Math.random() * 5000), parseInt(Math.random() * 000)], "255,255,255", "102,177,156	");
    }
    */
    console.log(squares.length);
}

function draw() {
    for (var i = 0; i < squares.length; i++) {
        squares[i].checkBeat();
        squares[i].display();
    }
}