function green(str) {
	return '\x1b[92m' + str + '\x1b[0m'
}

function red(str) {
	return '\x1b[91m' + str + '\x1b[0m'
}

for (let i = 0; i < __PassedTests__.length; i++) {
  const details = __PassedTests__[i];
  console.log(details["message"] + " " + green("PASSED"));
}

for (let i = 0; i < __FailedTests__.length; i++) {
  const details = __FailedTests__[i];
  console.log(details["message"] + " " + red("FAILED:"));
	console.log("    " + "Expect: " + green(details["expect"]))
	console.log("    " + "Got:    " + red(details["got"]))
}
