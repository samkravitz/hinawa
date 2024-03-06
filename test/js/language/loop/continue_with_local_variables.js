// This test was written after a bug with the compiler was discovered,
// when continue is used in loops that have local variables, the continue
// wouldn't pop the correct number of things off the stack. Thus, after
// the loop, there were too many variables on the stack, and
// local variables were being assigned to the wrong values

function main(a, b) {
  var bar = "bar";
  var n = null;
  var nums = [1, 2, 3, 4, 5, 6];
  var count = nums.length;

  for (var i = 0; i < count; i++) {
    var l1 = "l";
    var l2 = "l2";
    if (i < 4) {
      continue;
    }

    var l3 = "l3";
  }

  var after_loop = 13;
  var nums_copy = nums;
  print(after_loop);
  print(nums_copy);
  print(nums);
  print(count);
}

main("a", "b");
