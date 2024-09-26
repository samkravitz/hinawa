function main() {
  const obj = {};

  return function () {
    obj.foo = "bar";
    print(obj);

    try {
      obj = {};
    } catch (e) {
      print("caught attempt to assign to a constant!");
    }
  };
}

const closure = main();
closure();
