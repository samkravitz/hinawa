const obj = {
  foo: "bar"
};

const key = "key";
print(obj);
print(obj["foo"]);

print(obj["key"]);
print(obj[key]);

print((obj[key] = "value"));
print(obj);
