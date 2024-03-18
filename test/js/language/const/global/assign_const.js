const x = 10;

try {
  x = 20;
} catch (e) {
  print("caught attempt to assign to a constant!");
}
