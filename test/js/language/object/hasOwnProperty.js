var obj = {};
obj.property1 = 42;

print(obj.hasOwnProperty("property1"));
print(obj.hasOwnProperty("toString"));
print(obj.hasOwnProperty("hasOwnProperty"));

// test passing unusual values to hasOwnProperty
print(obj.hasOwnProperty());
print(obj.hasOwnProperty(undefined));
print(obj.hasOwnProperty(undefined, undefined));
