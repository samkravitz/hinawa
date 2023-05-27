try {
  var a = 1, b = 2, c = 3;
  var d = a + b;
  throw a;
} catch {
  var local = {};
  print("No exception var!");
}
