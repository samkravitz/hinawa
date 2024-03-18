const obj = {};
obj.foo = "bar";
print(obj);

try {
  obj = {};
} catch (e) {
  print("caught attempt to assign to a constant!");
}
