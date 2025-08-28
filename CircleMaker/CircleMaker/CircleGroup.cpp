#include "pch.h"

#include "CircleGroup.h"
#include "Circle.h"

CircleGroup::CircleGroup() {
	this->count = 0;
	this->selected = -1;
}

CircleGroup::~CircleGroup() {
	int i = this->count - 1;
	while (i >= 0) {
		delete this->circles[i];
		i--;
	}
}

int CircleGroup::Add(int x, int y, int radius) {
	this->circles[this->count] = new Circle(x, y, radius);
	this->selected = this->count;
	this->count++;
	return this->selected;
}

Circle* CircleGroup::GetAt(int index) {
	return this->circles[index];
}

void CircleGroup::Clear() {
	int i = this->count - 1;
	while (i >= 0) {
		delete this->circles[i];
		i--;
	}
	this->count = 0;
	this->selected = -1;
}

int CircleGroup::Select(int index) {
	this->selected = index;
	return this->selected;
}

Circle* CircleGroup::MakeCircumCircle() {
	Circle* circumCircle = NULL;

	if (this->count == 3) {
		int x1 = this->circles[0]->GetX();
		int y1 = this->circles[0]->GetY();
		int x2 = this->circles[1]->GetX();
		int y2 = this->circles[1]->GetY();
		int x3 = this->circles[2]->GetX();
		int y3 = this->circles[2]->GetY();
		int distance = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
		if (distance != 0) {
			// 외심을 구한다.
			int uX = ((x1 * x1 + y1 * y1) * (y2 - y3) + (x2 * x2 + y2 * y2) * (y3 - y1) + (x3 * x3 + y3 * y3) * (y1 - y2)) / distance;
			int uY = ((x1 * x1 + y1 * y1) * (x3 - x2) + (x2 * x2 + y2 * y2) * (x1 - x3) + (x3 * x3 + y3 * y3) * (x2 - x1)) / distance;

			// 외심을 중심으로 외접원을 만든다.
			double r = sqrt((x1 - uX) * (x1 - uX) + (y1 - uY) * (y1 - uY));
			int radius = static_cast<int>(r + 0.5);

			circumCircle = new Circle(uX, uY, radius);
		}
	}

	return circumCircle;
}