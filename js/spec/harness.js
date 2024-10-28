const __TestResults__ = {};

const defaultSuiteMessage = "__TOP_LEVEL__";
let suiteMessage = defaultSuiteMessage;

const describe = (message, callback) => {
  suiteMessage = message;
  if (!__TestResults__[suiteMessage]) __TestResults__[suiteMessage] = {};

  try {
    callback();
  } catch (e) {
    console.log("Suite " + suiteMessage + " failed to run.");
  }

  suiteMessage = defaultSuiteMessage;
};

const test = (message, callback) => {
  if (!__TestResults__[suiteMessage]) __TestResults__[suiteMessage] = {};

  const suite = __TestResults__[suiteMessage];

  try {
    callback();
    suite[message] = {
      passed: true
    };
  } catch (e) {
    suite[message] = {
      passed: false,
      details: e
    };
  }
};

const expect = (value) => {
  return new Expector(value);
};

function Expector(target, inverted) {
  this.target = target;
  this.inverted = !!inverted;

  this.toEqual = function (value) {
    if (value !== this.target) {
      const details = "Expected: " + target + " got: " + value;
      throw details;
    }
  };

  this.not = function () {
    return new Expector(this.target, !this.inverted);
  };
}
