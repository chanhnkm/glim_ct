#ifndef _CIRCLEGROUP_H
#define _CIRCLEGROUP_H

class Circle;

class CircleGroup {
public:
	CircleGroup();
	~CircleGroup();

	int Add(int x, int y, int radius);
	Circle* GetAt(int index);
	void Clear();
	int Select(int index);

	Circle* MakeCircumCircle();

	int GetCount() const;
	int GetSelected() const;

private:
	Circle* circles[3];
	int count;
	int selected;
};

inline int CircleGroup::GetCount() const {
	return this->count;
}
inline int CircleGroup::GetSelected() const {
	return this->selected;
}

#endif //_CIRCLEGROUP_H