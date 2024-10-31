// ++

describe("post-increment", () => {
  test("x++ increments and then uses the variable", () => {
    let x = 1;
    expect(x).toEqual(1);
    expect(x++).toEqual(1);
    expect(x).toEqual(2);
  });
});

describe("pre-increment", () => {
  test("++x uses the variable and then increments", () => {
    let x = 1;
    expect(x).toEqual(1);
    expect(++x).toEqual(2);
    expect(x).toEqual(2);
  });
});
