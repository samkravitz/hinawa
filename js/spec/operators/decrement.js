// --

describe("post-decrement", () => {
  test("x-- decrements and then uses the variable", () => {
    let x = 1;
    expect(x).toEqual(1);
    expect(x--).toEqual(1);
    expect(x).toEqual(0);
  });
});

describe("pre-decrement", () => {
  test("--x uses the variable and then decrements", () => {
    let x = 1;
    expect(x).toEqual(1);
    expect(--x).toEqual(0);
    expect(x).toEqual(0);
  });
});
