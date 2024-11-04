describe("Object#hasOwnProperty", () => {
  const obj = { foo: 42 };

  test("it returns true for its own properties", () => {
    expect(obj.hasOwnProperty("foo")).toBeTrue();
  });

  test("it returns false for a property it does not have", () => {
    expect(obj.hasOwnProperty("bar")).toBeFalse();
  });

  test("it returns false for properties on its prototype", () => {
    expect(obj.hasOwnProperty("toString")).toBeFalse();
  });

  test("it returns false for static methods on object", () => {
    expect(obj.hasOwnProperty("hasOwnProperty")).toBeFalse();
  });

  test("it can handle being passed unusual values", () => {
    expect(obj.hasOwnProperty()).toBeFalse();
    expect(obj.hasOwnProperty("")).toBeFalse();
    expect(obj.hasOwnProperty(undefined)).toBeFalse();
    expect(obj.hasOwnProperty(undefined, undefined)).toBeFalse();
  });
});
