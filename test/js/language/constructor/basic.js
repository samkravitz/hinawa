function Foo() {
  this.x = 123;
}

function Bar(value) {
  this.value = value;
}

var foo1 = new Foo(); // with parens
var foo2 = new Foo; // without parens

var bar1 = new Bar(2);
var bar2 = new Bar;
print(foo1);
print(foo2);
print(bar1);
print(bar2);
