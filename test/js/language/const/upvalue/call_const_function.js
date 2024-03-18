function main() {
  const getTwo = () => {
    return 2;
  };

  const getThree = () => {
    return 3;
  };

  return function () {
    const two = getTwo();
    const three = getThree();

    print(two);
    print(three);
  };
}

const closure = main();
closure();
