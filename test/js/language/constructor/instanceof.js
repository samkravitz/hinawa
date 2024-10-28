function Car(make, model) {
  this.make = make;
  this.model = model;
}

var car = new Car("Toyota", "Prius");
print(car instanceof Car);
print(car instanceof Object);
print(Car instanceof Object);
print(Car instanceof Car);
