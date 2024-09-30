for (let i = 0; i < __PassedTests__.length; i++) {
  const details = __PassedTests__[i];
  console.log(details["message"], "PASSED");
}

for (let i = 0; i < __FailedTests__.length; i++) {
  const details = __FailedTests__[i];
  console.log(details["message"], "FAILED:", details["details"]);
}
