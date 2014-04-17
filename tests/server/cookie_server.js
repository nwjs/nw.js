var http = require('http');
var url = require('url');

var expires_second = 86400000;

var id_list = [];

var request_listener = function(req,res){
  var location = url.parse(req.url,true);
  var id = undefined;
  var lang = undefined;
  var cookies = {};
  var expires = new Date(Date.now()+expires_second).toGMTString();
  var set_cookies = null;

  var cookie_header = req.headers['cookie'];
  
  if (!cookie_header || cookie_header.length === 0){
    id = Math .random();
    lang = cookies["lang"]||"balabala";
  } else {
    var cookie_tokens = cookie_header.split("; ");
    for (var i=0;i<cookie_tokens.length;++i){
      var nvp = cookie_tokens[i].split('=');
      var name = nvp[0]||undefined;
      var value = nvp[1]||undefined;
      if (!name){
        continue;
      }
      cookies[name] = value;
    }

    id = cookies["id"]||Math.random();
    lang = cookies["lang"]||"balabala";
    if (lang=== "undefined" || lang===undefined){
      lang = "balabala";
    }
  }

  if (id_list.indexOf(id+'')===-1){
    id_list.push(id+'');
    set_cookies = [
      "id="+id+"; Path=/; Expires="+expires+";",
      "key1=value1; Path=/; Expires="+expires+"; HttpOnly",
      "key2=value2; Path=/; Expires="+expires+";",
      "key3=value3; Path=/; Expires="+expires+"; ",
      "lang="+lang+"; Path=/; Expires="+expires+"; ",
    ];
  }

  //do not let nw cache anything unless you need it,
  //when you restart node-webkit, it may load content from the cache,
  //this usually should not allowed when doing test.
  var headers = {
    "Content-type":"text/plain",
    "Pragma":"no-cache",
    "Cache-Control":"no-cache",
    "Expires": new Date(0).toGMTString()
  };

  if (set_cookies && set_cookies.length >0){
    headers["Set-Cookie"] = set_cookies;
  }
  res.writeHead(200,headers);
  var html = JSON.stringify(cookies);
  res.end(html);
};

var server = http.createServer(request_listener);
server.listen(8125);
console.log("Cookie server is running!");
