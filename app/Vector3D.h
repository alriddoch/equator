// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef VECTOR_3D_H
#define VECTOR_3D_H

#include <Atlas/Message/Object.h>
#include <Eris/Types.h>

#include <cmath>
#include <algorithm>
#include <iosfwd>

using std::cos;
using std::sin;

using Atlas::Message::Object;

class Vector3D : protected Eris::Coord {
    // double x,y,z;
    bool _set;
  public:
    static const int cX = 0;    // Used to indicate which axis
    static const int cY = 1;
    static const int cZ = 2;

    Vector3D() : Coord(0, 0, 0), _set(false) { }
    Vector3D(double size) : Coord(size, size, size), _set(true) { }
    Vector3D(double x, double y, double z) : Coord(x, y, z), _set(true) { }
    Vector3D(const Eris::Coord & coord) : Coord(coord), _set(true) { }
    Vector3D(const Object::ListType & vector) : Coord(vector), _set(true) { }

    double X() const { return x; }
    double Y() const { return y; }
    double Z() const { return z; }

    bool operator==(const Vector3D & other) const {
        //"Check if two vector are equal";
        return ((x==other.x) && (y==other.y) && (z==other.z));
    }

    const Vector3D operator+(const Vector3D & other) const {
        // Add two vectors
        return Vector3D(x+other.x, y+other.y, z+other.z);
    }

    Vector3D & operator+=(const Vector3D & other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    const Vector3D operator+(double other) const {
        // Increaese size (in all direction for boxes)
        return Vector3D(x+other, y+other, z+other);
    }

    Vector3D & operator+=(double other) {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    const Vector3D operator-(const Vector3D & other) const {
        // Subtract two vectors
        return Vector3D(x-other.x, y-other.y, z-other.z);
    }

    Vector3D & operator-=(const Vector3D & other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    const Vector3D operator-(double other) const {
        // Decrease size (in all direction for boxes)
        return Vector3D(x-other, y-other, z-other);
    }

    Vector3D & operator-=(double other) {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    const Vector3D operator*(double other) const {
        // Multiply vector by number
        return Vector3D(x*other,y*other,z*other);
    }

    Vector3D & operator*=(double other) {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    const Vector3D operator/(double other) const {
        // Divide vector by number
        return Vector3D(x/other,y/other,z/other);
    }

    Vector3D & operator/=(double other) {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    operator bool() const {
        return _set;
    }

    double & operator[](int index) {
	switch(index) {
            case cX:
                return x;
            case cY:
                return y;
            case cZ:
                return z;
            default:
                //Throw an exception here maybe
                return z;
	}
    }

    void set() {
        _set = true;
    }

    double dot(const Vector3D & v) const {
        //"Dot product of a vector";
        return x * v.x + y * v.y + z * v.z;
    }

    const Vector3D cross(const Vector3D & v) const {
        //"Cross product of a vector";
        return Vector3D(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }

    void rotatex(double angle) {
        //"Rotate a vector around x axis by angle in radians" ;
        z = cos(angle) * z + sin(angle) * x;
        x = sin(angle) * z + cos(angle) * x;
    }

    void rotatey(double angle) {
        //"Rotate a vector around y axis by angle in radians"        ;
        y = cos(angle) * y + sin(angle) * z;
        z = sin(angle) * y + cos(angle) * z;
    }

    void rotatez(double angle) {
        //"Rotate a vector around z axis by angle in radians" ;
        x = cos(angle) * x + sin(angle) * y;
        y = sin(angle) * x + cos(angle) * y;
    }

    double mag() const {
        //"Find the magnitude of a vector";
        return sqrt(x*x + y*y + z*z);
    }

    double angle(const Vector3D & v) const {
        //"Find the angle between two vectors";
        double d = v.x * x + v.y * y + v.z * z;
        return d / (v.mag() * mag());
    }

    const Vector3D unitVector() const {
        //"return the unit vector of a vector";
	// This is could throw a wobbly
	return operator/(mag());
    }

    const Vector3D unitVectorTo(const Vector3D & v) const {
        // return the unit vector in the direction of another vector;
        Vector3D difference_vector = v - (*this);
        return difference_vector.unitVector();
    }

    double distance(const Vector3D & v) const {
        //"Find the distance between two vectors";
        return sqrt((x - v.x)*(x - v.x) + (y - v.y)*(y - v.y) + (z - v.z)*(z - v.z));
    }

    const Object asObject() const {
        Object::ListType coords;
        coords.push_back(Object(x));
        coords.push_back(Object(y));
        coords.push_back(Object(z));
        return Object(coords);
    }


    friend std::ostream & operator<<(std::ostream& s, const Eris::Coord& v);
    friend std::ostream & operator<<(std::ostream& s, const Vector3D& v);
};

#endif // VECTOR_3D_H
