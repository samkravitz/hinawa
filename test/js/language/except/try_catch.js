try {
  var a = "hello";
  var b = "world";
  var c = {};
  var d = 1 + 2;
  throw c;
} catch (e) {
  print(e);
  print("caught!");
}
