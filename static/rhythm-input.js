var socket = io.connect(window.origin);
$("#recording-controls").hide();
var pattern = new tappy.Rhythm();
var timeout;
$('#tap-pad').on('mousedown touchstart', function(){
  clearTimeout(timeout);
  pattern.tap();
  console.log(pattern.length);
  timeout = setTimeout(function(){
    pattern.done();console.log("Recorded");
    $("#tap-pad").hide();
    display();
  }, 2000);
  console.log(pattern.duration);
});
function display() {
  pattern.playback(function(i) {
    $("#recording-controls").hide();
    $("#main-wrapper").append('<div class="expanding-circle"></div>');
  },
  function(){
    $("#recording-controls").fadeIn();
    setTimeout(function(){ $(".expanding-circle").remove();}, 1000);
  }, 1.2);
}
$('#play').on('click', function(e) {
  e.preventDefault();
  display();
});
$("#record").on('click', function(e){
  e.preventDefault();
  pattern = new tappy.Rhythm();
  $("#recording-controls").fadeOut();
  $("#tap-pad").show();
});

$("#illuminate").on('click', function(e){
  e.preventDefault();
  socket.emit('illuminate', pattern)
});
