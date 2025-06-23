global.console.log('--> start');

const { Session } = require('node:inspector/promises');
const cons = require('node:inspector').console;
const inspector = require('node:inspector');

var session;

cons.log('--> loaded');
//inspector.open(9222);
(async () => {
  try {
    cons.log('--> new session');
    session = new Session();
    console.log('--> connect');
    session.connect();
    console.log('--> await');
    // Listen for console API calls
    session.on('Runtime.consoleAPICalled', (event) => {
      const { type, args, timestamp } = event.params;
      const message = args.map(arg => arg.value).join(' ');
      if (typeof document != 'undefined')
        document.write(`<p>console log: ${message}</p>`);
    });
    await session.post('Runtime.enable');
    if (chrome.test) {
        chrome.test.sendMessage("Launched");
    }
    cons.log('--> here');
  } catch (error) {
    console.error(error);
  }
})();

async function test() {
    const result = await session.post('Runtime.evaluate', { expression: '2 + 2' });

    if (chrome.test) {
      if ( result.result.value == 4 )
        chrome.test.sendMessage("Passed");
    }
    cons.log( JSON.stringify(result) );
}
