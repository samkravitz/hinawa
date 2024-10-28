var obj = { x: "this" };

obj.fn = function () {
  print(this.x);
};

var method = obj.fn;

method();
obj.x = "that";
method();
