const http = require("http");
const util = require("util");
const multiparty = require("./multiparty");

const port = 2345;

http
  .createServer(function (req, res) {
    if (
      req.url ===
        "/reportloooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooongurl" &&
      req.method === "POST"
    ) {
      const form = new multiparty.Form();

      form.parse(req, function (err, fields, files) {
        res.writeHead(200, { "content-type": "text/plain" });
        res.write("received upload:\n\n");
        console.log(
          util.inspect({ fields: fields, files: JSON.stringify(files) })
        );

        console.log(`\n\n`);

        res.end();
      });
    }
  })
  .listen(port, () => {
    console.log(`nwjs crash report server start at %s`, port);
  });
