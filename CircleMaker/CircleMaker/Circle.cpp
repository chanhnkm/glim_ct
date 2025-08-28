#include "pch.h"

#include "Circle.h"

Circle::Circle(int x, int y, int radius) {
	this->x = x;
	this->y = y;
	this->radius = radius;
}

Circle::~Circle() {

}

void Circle::Move(int x, int y) {
	this->x = x;
	this->y = y;
}