document.getElementById("color").focus()

$(document).keydown (evt) ->
  if evt.keyCode == 13
    $("body").css("background-color", $("#color").val())
