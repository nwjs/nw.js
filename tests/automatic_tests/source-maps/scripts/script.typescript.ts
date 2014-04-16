/// <reference path="jquery.d.ts" />

// on load, put focus on the input
document.getElementById("color").focus();

$(document).keydown(function(evt) {

  // only if the user keypress is ENTER
  if (evt.keyCode == 13) {
    // css color names and hex code
    $("body").css("background-color", $("#color").val());
  }

});