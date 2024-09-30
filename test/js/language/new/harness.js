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
      details: e
    });
  }
};

function Expector(target) {
  this.target = target;

  this.toEqual = function (value) {
    if (value !== this.target) {
      let message = "Expect: " + target + " got: " + value;
      throw message;
    }
  };
}
