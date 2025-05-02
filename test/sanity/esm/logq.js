'use strict';

const queue = [];
const waitingResolvers = [];

function append(item) {
  if (typeof item !== 'string') {
    throw new TypeError('Queue item must be a string.');
  }

  if (waitingResolvers.length > 0) {
    const resolve = waitingResolvers.shift();
    resolve(item);
  } else {
    queue.push(item);
  }
}

function extractHead() {
  if (queue.length > 0) {
    const headItem = queue.shift();
    return Promise.resolve(headItem);
  } else {
    return new Promise((resolve) => {
      waitingResolvers.push(resolve);
    });
  }
}

function getLength() {
  return queue.length;
}

function isEmpty() {
  return queue.length === 0;
}

module.exports = {
  append,
  extractHead,
  getLength,
  isEmpty,
};
