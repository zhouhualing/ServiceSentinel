#ifndef INCLUDED_STLUTILS_H
#define INCLUDED_STLUTILS_H
#pragma once
#include <vector>
#include <Windows.h>
#include <unordered_map>
using namespace std;
//
//typedef string Key;
//typedef HANDLE Value;
//template<typename myMap>
//std::vector<typename myMap::key_type> Keys(const myMap& m);
//template<typename myMap>
//std::vector<typename myMap::mapped_type> Values(const myMap& m);
//int Transform(unordered_map<Key, Value> map, vector<Key>& keys, vector<Value>& values);

template<typename myMap>
std::vector<typename myMap::key_type> Keys(const myMap& m)
{
	std::vector<typename myMap::key_type> r;
	r.reserve(m.size());
	for (const auto&kvp : m)
	{
		r.push_back(kvp.first);
	}
	return r;
}

template<typename myMap>
std::vector<typename myMap::mapped_type> Values(const myMap& m)
{
	std::vector<typename myMap::mapped_type> r;
	r.reserve(m.size());
	for (const auto&kvp : m)
	{
		r.push_back(kvp.second);
	}
	return r;
}
#endif