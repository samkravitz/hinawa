function Foo() {
	var d = (this.bar = [0]);
	print(d)
	print(this)
}

new Foo()

var foo = {}
var b = foo.bar = 'baz'
print(b)
