const __PassedTests__ = [];
const __FailedTests__ = [];

const expect = (value) => {
  return new Expector(value);
};

const test = (message, callback) => {
  try {
    callback();
    __PassedTests__.push({
      message: message
    });
  } catch (e) {
    __FailedTests__.push({
      message: message,
      expect: e.expect,
      got: e.got
    });
  }
};

function Expector(target) {
  this.target = target;

  this.toEqual = function (value) {
    if (value !== this.target) {
      const details = {
        expect: target,
        got: value
      };

      throw details;
    }
  };
}
