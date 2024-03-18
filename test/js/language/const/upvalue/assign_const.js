function main() {
  const x = 10;

  return function () {
    try {
      x = 20;
    } catch (e) {
      print("caught attempt to assign to a constant!");
    }
  };
}

const closure = main();
closure();
