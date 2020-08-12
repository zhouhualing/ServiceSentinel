#include"stdafx.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "StlUtils.h"
using namespace std;

//auto key_selector = [](auto pair) {return pair.first; };
//auto value_selector = [](auto pair) {return pair.second; };
//
//int Transform(unordered_map<Key, Value> map, vector<Key>& keys, vector<Value>& values)
//{
//	
//	// Vectors to hold keys and values
//	keys.resize(map.size());
//	keys.clear();
//	values.resize(map.size());
//	values.clear();
//	transform(map.begin(), map.end(), keys.begin(), key_selector);
//	transform(map.begin(), map.end(), values.begin(), value_selector);
//	return 1;
//}
//
//template<typename myMap>
//std::vector<typename myMap::key_type> Keys(const myMap& m)
//{
//	std::vector<typename myMap::key_type> r;
//	r.reserve(m.size());
//	for (const auto&kvp : m)
//	{
//		r.push_back(kvp.first);
//	}
//	return r;
//}
//
//template<typename myMap>
//std::vector<typename myMap::mapped_type> Values(const myMap& m)
//{
//	std::vector<typename myMap::mapped_type> r;
//	r.reserve(m.size());
//	for (const auto&kvp : m)
//	{
//		r.push_back(kvp.second);
//	}
//	return r;
//}