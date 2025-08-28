#ifndef _CIRCLE_H
#define _CIRCLE_H

class Circle {
public:
	Circle(int x, int y, int radius);
	~Circle();

	void Move(int x, int y);

	int GetX() const;
	int GetY() const;
	int GetRadius() const;

private:
	int x;
	int y;
	int radius;
};

inline int Circle::GetX() const {
	return this->x;
}

inline int Circle::GetY() const {
	return this->y;
}

inline int Circle::GetRadius() const {
	return this->radius;
}

#endif //_CIRCLE_H