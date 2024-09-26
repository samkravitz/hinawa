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

test("1 + 1 = 2", () => {
  expect(1 + 1).toEqual(2);
});

test("true does not equal false", () => {
  expect(true).toEqual(false);
});

for (let i = 0; i < __PassedTests__.length; i++) {
  const details = __PassedTests__[i];
  console.log(details["message"], "PASSED");
}

for (let i = 0; i < __FailedTests__.length; i++) {
  const details = __FailedTests__[i];
  console.log(details["message"], "FAILED:", details["details"]);
}
