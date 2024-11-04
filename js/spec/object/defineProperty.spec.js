describe("Object#defineProperty", () => {
  const o = {};

  describe("writable", () => {
    test("it allows changing a property that is configured as writable", () => {
      Object.defineProperty(o, "a", {
        value: 37,
        writable: true,
      });

      expect(o.a).toEqual(37);
      o.a = 42;
      expect(o.a).toEqual(42);
    });

    test("it does not allow changing a property that is configured as not writable", () => {
      Object.defineProperty(o, "b", {
        value: "foo",
        writable: false,
      });

      expect(o.b).toEqual("foo");
      o.b = "bar";
      expect(o.b).toEqual("foo");
    });
  });

  describe("defaults", () => {
    test("it defaults the value to undefined", () => {
      Object.defineProperty(o, "c", {});
      expect(o.c).toBeUndefined();
    });

    test("it defaults the writable key to false", () => {
      Object.defineProperty(o, "d", {});
      o.d = true;
      expect(o.d).toBeUndefined();
    });
  });
});
