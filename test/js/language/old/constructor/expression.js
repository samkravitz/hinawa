function Bar() {
  this.foo = "foo";
}

var constructor = Bar;
var bar = new constructor();

print(bar);
