function canDrive(age) {
  if (age >= 16) {
    print(age, "can drive");
  } else {
    print(age, "can't drive");
  }
}

canDrive(15);
canDrive(16);
canDrive(17);
canDrive(100);

{
  var x = 0;
  if (false) x = 1;
  else if (false) x = 2;
  else x = 3;

  var c = x;
  print(c);
}
