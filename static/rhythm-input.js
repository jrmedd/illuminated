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
            $(this).html("Your illumination is appearing");
            $(this).fadeIn();
        });
    }
});

$('#tap-pad').on('click', function(){
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
