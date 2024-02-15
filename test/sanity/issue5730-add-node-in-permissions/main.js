
   var wv = document.querySelector('webview');
   document.getElementById("request1").innerHTML = JSON.stringify(wv.request);

   setTimeout(function(){
      document.getElementById("request2").innerHTML = JSON.stringify(wv.request);
   }, 500);

   
