$("#recording-controls").hide();

var socket = io.connect(window.origin);
var pattern = new tappy.Rhythm();
var timeout;

socket.on('connect', function(){
    sessionid = this.id;
});

socket.on('queueAlert', function(data){
    if (sessionid == data.session_illuminating) {
        $("#confirm-illumination h1").fadeOut(function() {
            $(this).html("Your rhythm is appearing. You can close this window.");
            $(this).fadeIn();
            setTimeout(function(){location.reload()}, 3000);
        });
    }
    var positionInQueue = data.session_queue.indexOf(sessionid);
    var lengthOfQueue = data.session_queue.length;
    if (positionInQueue > 0) {
        $("#confirm-illumination h1").fadeOut(function () {
            if (positionInQueue > 1) {
                $(this).html("Your rhythm is in the queue. There are " + (positionInQueue) + " rhythms in front of yours.");
            }
            else {
                $(this).html("Your rhythm is in the queue. There is " + (positionInQueue) + " rhythm in front of yours.");
            }
            $(this).fadeIn();
        });
    }
});

$.getJSON('/prompt', function(data){
    $("#welcome h4").html(data.loaded_prompt);
})


$('#tap-pad').on('touchstart mouseup', function(e){
    e.stopPropagation();
    e.preventDefault();
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
    $("#welcome").fadeOut();
    $("#recording-controls").hide();
    $("#main-wrapper").append('<div class="expanding-circle"></div>');
  },
  function(){
    $("#recording-controls").fadeIn();
    setTimeout(function(){ $(".expanding-circle").remove();}, 1000);
  }, 1);
}
$('#play').on('click', function(e) {
  e.preventDefault();
  display();
});
$("#record").on('click', function(e){
  e.preventDefault();
  pattern = new tappy.Rhythm();
  $("#recording-controls").fadeOut(function(){
    $("#tap-pad").fadeIn();
  });
});
$("#illuminate").on('click', function(e){
  e.preventDefault();
  $("#recording-controls").fadeOut(function(){
    $("#confirm-illumination").fadeIn();
    $.ajax({
      type: "POST",
      url: 'illuminate',
      contentType: "application/json; charset=utf-8",
      data: JSON.stringify({'_taps':pattern._taps,'duration':pattern.duration,'session':sessionid}),
      success: function() {
      console.log("Illuminated");
    }
    });
  });
});
