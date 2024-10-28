function f4() {
  throw new Error("caught!");
}

function f3() {
  f4();
}

function f2() {
  f3();
}

function f1() {
  f2();
}

function f0() {
  f1();
}

try {
  f0();
} catch (e) {
  print(e.stack);
  print(e.message);
}
