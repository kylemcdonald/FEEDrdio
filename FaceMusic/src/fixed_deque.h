#pragma once

#include "ofMain.h"

template <class T>
class fixed_deque: public deque<T> {
public:
	fixed_deque()
	:maxSize(0) {
	}
	void setMaxSize(int maxSize) {
		this->maxSize = maxSize;
	}
	void push_back(const T& x) {
		deque<T>::push_back(x);
		while(deque<T>::size() > maxSize) {
			deque<T>::pop_front();
		}
	}
private:
	int maxSize;
};