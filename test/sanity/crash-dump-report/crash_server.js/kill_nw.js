/**
 * clear the NW.js process for old nwjs version
 * @author ostwindli
 */
const cp = require("child_process");
const exeStr = `ps -ef | grep nwjs | grep -v grep|awk '{print $2}'`;
(async function () {
  let pids = cp.execSync(exeStr).toString().split(/\n/);
  pids = pids.filter((pid) => pid);
  pids
    .map((pid) => {
      return parseInt(pid);
    })
    .forEach((pid) => {
      process.kill(pid, "SIGTERM");
    });

  pids.length && console.log(`clean nw pids: `, pids);
})();
