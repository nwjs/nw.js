function chooseDesktopMedia(){
    if(nw.Screen.Init){
        nw.Screen.Init();
    }
    nw.Screen.chooseDesktopMedia(["screen"],function(streamId){
        if(streamId) {
            var conf = {
                mandatory: {
                    chromeMediaSource: 'desktop',
                    chromeMediaSourceId: streamId,
                    maxWidth: 1920,
                    maxHeight: 1080
                },
                optional: []
            };
            console.log("[nw.chooseDesktopMedia] stream id:" + streamId);
            showScreenShare({audio: false, video: conf});
        }
    })
}

function showScreenShare(conf){
    var ve = document.getElementById("screen-share");

   navigator.mediaDevices.getUserMedia(conf)
    .then(function(stream){
        var url = window.URL.createObjectURL(stream);
        ve.src = url;
        if (chrome.test) {
           chrome.test.sendMessage("Pass");
        }
    })
    .catch(function(e){
        console.log(e);
        if (chrome.test) {
           chrome.test.sendMessage("Fail");
        }else{
          alert(e);
        }
    });

}
