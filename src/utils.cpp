//
// Created by marco on 23/01/20.
//

#ifndef C___UTILS_CPP
#define C___UTILS_CPP

#include <vector>
#include <algorithm>
#include <cstdint>

using VectorizableBool=std::uint8_t;

template<typename T>
bool contains(const std::vector<T> &vector, const T &value) {
	return std::find(vector.begin(), vector.end(), value) != vector.end();
}

#endif //C___UTILS_CPP
