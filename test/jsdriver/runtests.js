const { readdirSync } = require('fs');
const shell = require('shelljs');
const {default: PQueue} = require('p-queue');
const {chunksToLinesAsync, chomp} = require('@rauschma/stringio');
const {spawn} = require('child_process');
const path = require('path');
const fs = require('fs');
const util = require('util');
const net = require('net');

shell.config.silent = true;

const getDirectories = source =>
      readdirSync(source, { withFileTypes: true })
      .filter(dirent => dirent.isDirectory())
      .map(dirent => dirent.name);

let known_flaky = new Set([]);

function getFreePort() {
  return new Promise((resolve, reject) => {
    const server = net.createServer();
    server.unref(); // Allows the program to exit if this is the only server running
    server.on('error', (err) => {
      // If EADDRINUSE, it means the port was taken between listen(0) and server.address()
      // or another specific error. We can retry or just reject.
      if (err.code === 'EADDRINUSE') {
        console.warn('getFreePort: EADDRINUSE, trying again shortly.');
        // Retry logic could be added here, or simply let it try again in the pool initialization
        setTimeout(() => getFreePort().then(resolve).catch(reject), 50 + Math.random() * 100);
      } else {
        reject(err);
      }
    });
    server.listen(0, () => { // 0 means OS will assign an available port
      const { port } = server.address();
      server.close((err) => { // Ensure server is closed before resolving
        if (err) { // Error on close might happen if port was already taken
          reject(err);
        } else {
          resolve(port);
        }
      });
    });
  });
}

if (require.main == module) {
  (async() => {
    let ret = await run_tests('content/nw/test/sanity', (err, testcase) => {
      if (err) {
        console.log(`FAILED: ${testcase}`);
        fs.writeFile(`${testcase}.txt`, err, (e) => {});
      }
      else
        console.log(`PASSED: ${testcase}`);
    }, undefined, 5);
    console.log(`${ret.passed.length} passed, ${ret.failed.length} failed.`);
    if (ret.failed.length > 0) {
      let test_ret2 = await run_tests('content/nw/test/sanity', (err, testcase) => {
        if (err) {
          console.log(`FAILED: ${testcase}`);
          //fs.writeFile(`${testcase}.${updated_hash}.txt`, err, (e) => {});
        }
      }, ret.failed, 1);
      for (const testcase of test_ret2.failed) {
        if (!known_flaky.has(testcase)) {
          console.log(`test failed: ${testcase}`);
          process.exit(1);
        }
      }
    }
  })();
}

async function run_tests(test_dir = 'content/nw/test/sanity', cb, testcases, concurrency = 5) {
  const queue = new PQueue({concurrency: concurrency});

  if (!testcases)
    testcases = getDirectories(test_dir);
  let env = Object.create(process.env);
  let ret = { passed: [], failed: [] };
  env.PYTHONPATH = path.resolve(path.join(process.cwd(), 'third_party/webdriver/pylib/'));
  env.CHROMEDRIVER = path.resolve(path.join(process.cwd(), 'outst/nw/chromedriver'));
  env.GYP_DEFINES = 'target_arch=x64 clang=1 nwjs_sdk=1 disable_nacl=0 buildtype=Official';
  env.CC = 'clang';
  env.CXX = 'clang++';

  const availablePorts = [];

  async function initializePortPool(count) {
    if (count <= 0) {
      console.log("Concurrency is 0 or less, no ports will be initialized for a pool.");
      return [];
    }
    //console.log(`Initializing port pool: attempting to secure ${count} free ports...`);
    const securedPorts = new Set();
    let attempts = 0;
    const maxAttemptsPerPort = 5; // Max attempts to find a single port
    const overallMaxAttempts = count * maxAttemptsPerPort * 2; // Overall safety net

    while (securedPorts.size < count && attempts < overallMaxAttempts) {
      attempts++;
      try {
        const port = await getFreePort();
        if (!securedPorts.has(port)) {
          securedPorts.add(port);
          //console.log(`[PortPool] Secured port ${port}. Total secured: ${securedPorts.size}/${count}`);
        }
      } catch (portError) {
        console.warn(`[PortPool] Attempt ${attempts}: Failed to get a free port: ${portError.message}.`);
        if (securedPorts.size < count) { // Only wait if we still need more ports
            await new Promise(resolve => setTimeout(resolve, 100 + Math.random() * 150));
        }
      }
    }

    if (securedPorts.size < count) {
      console.warn(`[PortPool] WARNING: Could only secure ${securedPorts.size} out of ${count} requested ports.`);
      if (securedPorts.size === 0 && count > 0) {
        throw new Error("[PortPool] CRITICAL: Failed to secure any ports. Aborting test run.");
      }
    } else {
      //console.log(`[PortPool] Successfully secured ${securedPorts.size} ports.`);
    }
    return Array.from(securedPorts);
  }
  const initialPool = await initializePortPool(concurrency);
  availablePorts.push(...initialPool);
  //console.log("[PortPool] Initial available ports:", availablePorts);
  await Promise.all(testcases.map(testcase =>
    queue.add(async () => {
      let allocatedPort = null;
      if (concurrency > 0) { // Only try to allocate if concurrency suggests ports are used
        if (availablePorts.length > 0) {
          allocatedPort = availablePorts.pop(); // Take a port from the pool
          //console.log(`[${testcase}] Acquired port: ${allocatedPort}. Ports available in pool: ${availablePorts.length}`);
        } else {
          // This condition means all pre-allocated ports are currently in use.
          // Since p-queue manages concurrency, this implies a mismatch or an issue if pool size was < concurrency.
          // Or, if a test failed to release a port.
          console.warn(`[${testcase}] No pre-allocated port available from pool. Test will run without --chromedriver-port.`);
          // For robustness, you might try to get a new free port on-the-fly here if strictly needed,
          // but it complicates pool management if that new port isn't part of the original pool.
          // Alternatively, the task could wait for a port to be released.
          // For this setup, we rely on the initial pool size matching concurrency.
        }
      }
      try {
        let case_path = path.join(process.cwd(), test_dir, testcase);
        if (!fs.existsSync(path.join(case_path, '_skip')) &&
            fs.existsSync(path.join(case_path, 'test.py'))) {
          const pythonArgs = ['test.py'];
          if (allocatedPort !== null) {
            pythonArgs.push(`--chromedriver-port=${allocatedPort}`);
          }
          await exec_v('python3', pythonArgs, path.join(process.cwd(), test_dir, testcase), true, env);
          if (cb)
            cb(null, testcase);
          ret.passed.push(testcase);
        }
      } catch (e) {
        if (cb)
          cb(e.toString(), testcase);
        ret.failed.push(testcase);
      }  finally {
        if (allocatedPort !== null) {
          availablePorts.push(allocatedPort); // Release port back to the pool
          //console.log(`[${testcase}] Released port: ${allocatedPort}. Ports available in pool: ${availablePorts.length}`);
        }
      }
    })));
  return ret;
}

function sleep(ms) {
  return new Promise((_, reject) => setTimeout(() => reject(new Error('timeout')), ms));
}

async function* splitStreamByDelimiter(readable, delimiter, encoding = 'utf8') {
  // The rest of the function body remains exactly the same
  const decoder = new TextDecoder(encoding);
  let buffer = "";

  try {
    for await (const chunk of readable) {
      buffer += decoder.decode(chunk, { stream: true });

      let idx;

      while ((idx = buffer.indexOf(delimiter)) >= 0) {
        const segment = buffer.slice(0, idx);
        buffer = buffer.slice(idx + delimiter.length);
        yield segment;
      }
    }
  } catch (err) {
    console.error("Error reading from stream:", err);
    if (readable && !readable.destroyed) {
         readable.destroy(err instanceof Error ? err : new Error(String(err)));
    }
    throw err;
  }

  buffer += decoder.decode(); // Final flush

  let finalIdx; // Removed potential ': number' type hint for JS compatibility
  while ((finalIdx = buffer.indexOf(delimiter)) >= 0) {
      yield buffer.slice(0, finalIdx);
      buffer = buffer.slice(finalIdx + delimiter.length);
  }

  if (buffer.length > 0) {
      yield buffer;
  }
}

async function exec_v(cmd, arglist, cwd, flag_throw = true, env = process.env, timeout = 120000) {
  let source;
  let timer;
  let timed_out = false;
  let processOutputCollected = false; // Flag to indicate normal completion of output streams

  // This promise will be rejected by the timeout or resolved by normal process exit
  return new Promise((resolve, reject) => {
    source = spawn(cmd, arglist, {
      cwd: cwd,
      stdio: ['ignore', 'pipe', 'pipe'], // 'ignore' stdin, pipe stdout/stderr
      env: env,
      detached: true // Crucial for killing the process tree, especially on non-Windows
    });

    if (!source.pid) {
      const spawnError = new Error(`Failed to spawn process: ${cmd} ${arglist.join(' ')}`);
      if (flag_throw) {
        return reject(spawnError);
      } else {
        return resolve({ code: -1, stdout: "", stderr: `Failed to spawn process: ${spawnError.message}` });
      }
    }

    const pidToKill = source.pid; // Capture PID for use in timeout and cleanup

    const killProcess = (reason) => {
      if (source && !source.killed) {
        //console.log(`Attempting to kill process tree ${pidToKill} due to: ${reason}`);
        try {
          if (process.platform === 'win32') {
            // /T terminates the specified process and any child processes started by it.
            // /F forcefully terminates process(es).
            shell.exec(`taskkill /F /T /PID ${pidToKill}`, { silent: true, async: false });
          } else {
            // On POSIX, with detached: true, source.pid is the group leader (PGID).
            // Sending signal to -pgid kills the entire process group.
            shell.exec(`kill -- -${pidToKill}`, { silent: true, async: false });
          }
          //console.log(`Kill command issued for process tree ${pidToKill}.`);
          if (source) source.killed = true; // Mark as killed
        } catch (killError) {
          console.error(`Error attempting to kill process tree ${pidToKill}:`, killError.message);
          // Even if kill fails, we should proceed to resolve/reject for timeout
        }
      }
    };

    timer = setTimeout(() => {
      timed_out = true;
      killProcess("timeout");

      // Ensure streams are unlistened to prevent further processing
      if (source.stdout) source.stdout.destroy();
      if (source.stderr) source.stderr.destroy();

      const timeoutMessage = "TIMEOUT";
      // Capture whatever data has been collected so far
      const currentStdout = dataBuffer;
      const currentStderr = errorBuffer;
      const outputData = (currentStdout || "") + (currentStderr || "");

      clearTimeout(timer); // Clear timeout just in case

      if (flag_throw) {
        reject(new Error(`subprocess ${cmd} error exit ${timeoutMessage}, ${timeoutMessage}\n${outputData}`));
      } else {
        // Use a distinct exit code for timeout
        resolve({ code: -1, stdout: outputData, stderr: timeoutMessage });
      }
    }, timeout);

    let dataBuffer = "";
    let errorBuffer = "";

    const stdoutPromise = (async () => {
      try {
        for await (const line of chunksToLinesAsync(source.stdout)) {
          if (timed_out) break; // Stop processing if timeout occurred
          dataBuffer += line;
        }
      } catch (e) {
        // Ignore errors if timed_out, as streams might be abruptly closed
        if (!timed_out) console.error(`Stdout stream error for ${cmd} (${pidToKill}): ${e.message}`);
      }
    })();

    const stderrPromise = (async () => {
      try {
        for await (const line of chunksToLinesAsync(source.stderr)) {
          if (timed_out) break; // Stop processing if timeout occurred
          errorBuffer += line;
        }
      } catch (e) {
        if (!timed_out) console.error(`Stderr stream error for ${cmd} (${pidToKill}): ${e.message}`);
      }
    })();

    // Handle process exit/close
    source.on('close', async (code, signal) => {
      if (timed_out) return; // If timeout already handled, do nothing more here

      clearTimeout(timer); // Process finished, clear the timeout

      // Wait for stdout and stderr streams to finish processing any remaining data
      // This helps ensure all output is captured before resolving.
      try {
        await Promise.all([stdoutPromise, stderrPromise]);
      } catch (streamError) {
        // Should ideally not happen if individual stream readers catch errors
        console.error(`Error waiting for streams to close for ${cmd} (${pidToKill}):`, streamError);
      }
      processOutputCollected = true;

      // Defensive kill, especially if 'close' event fires but children might linger
      // (though `detached: true` and group kill should be effective).
      // This is more of a cleanup for potential zombies if the initial kill in timeout wasn't 100%
      // or if the process exited but left children.
      killProcess("process closed/exited");

      if (signal) { // Process was killed by a signal
        const errorMessage = `Process ${cmd} (${pidToKill}) terminated by signal: ${signal}`;
        if (flag_throw) {
          reject(new Error(`${errorMessage}\n${dataBuffer}${errorBuffer}`));
        } else {
          resolve({ code: code === null ? 1 : code, stdout: dataBuffer, stderr: errorBuffer + "\n" + errorMessage });
        }
      } else if (code !== 0) { // Process exited with an error code
        const errorMessage = `subprocess ${cmd} error exit ${code}`;
        if (flag_throw) {
          reject(new Error(`${errorMessage}, ${errorBuffer}\n${dataBuffer}`));
        } else {
          resolve({ code: code, stdout: dataBuffer, stderr: errorBuffer || `Exited with code ${code}` });
        }
      } else { // Process exited successfully
        if (flag_throw) {
          resolve(dataBuffer);
        } else {
          resolve({ code: 0, stdout: dataBuffer, stderr: errorBuffer });
        }
      }
    });

    source.on('error', (err) => {
      if (timed_out) return; // If timeout already handled, do nothing

      clearTimeout(timer);
      killProcess("spawn error"); // Attempt to clean up if process partially started

      const errorMessage = `Subprocess ${cmd} (${pidToKill}) failed to start or crashed: ${err.message}`;
      console.error(errorMessage, err);
      if (flag_throw) {
        reject(new Error(`${errorMessage}\n${dataBuffer}${errorBuffer}`));
      } else {
        resolve({ code: err.code || -1, stdout: dataBuffer, stderr: errorBuffer + "\n" + errorMessage });
      }
    });
  });
}

exports.run_tests = run_tests;
