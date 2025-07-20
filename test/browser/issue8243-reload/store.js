const getStorage = () => {
  if (typeof global !== 'undefined' && global.localStorage) {
    return global.localStorage;
  }
  throw new Error('localStorage is not available on the global object.');
};

const set = (key, value) => {
  const storage = getStorage();
  storage.setItem(key, JSON.stringify(value));
};

const get = (key) => {
  const storage = getStorage();
  const value = storage.getItem(key);
  try {
    return value ? JSON.parse(value) : null;
  } catch (e) {
    return value;
  }
};

module.exports = {
  set,
  get,
};
