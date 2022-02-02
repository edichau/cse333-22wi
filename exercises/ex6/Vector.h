// Copyright 2022 <Edison Chau>
#ifndef VECTOR_H_
#define VECTOR_H_

#include <iostream>

// A Vector is a vector in 3-space.
// Vector class
class Vector {
 public:
  // Default Constructor
  Vector() : x_(0.0), y_(0.0), z_(0.0) { }

  // Construct (x,y,z)
  Vector(const float x, const float y, const float z) : x_(x), y_(y), z_(z) { }

  // Copy
  Vector(const Vector& v) : x_(v.x_), y_(v.y_), z_(v.z_) { }

  // Destructor
  ~Vector() { }

  // Getters
  float get_x() const { return x_; }
  float get_y() const { return y_; }
  float get_z() const { return z_; }

  // Assignment operator
  Vector& operator=(const Vector& v);

  // Updating assignment
  Vector& operator+=(const Vector& v);
  Vector& operator-=(const Vector& v);

  // Dot product
  float operator*(const Vector& v) const;

 private:
  // the x, y, and z.
  float x_, y_, z_;
};

#endif  //  VECTOR_H_
