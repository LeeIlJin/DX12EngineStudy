#pragma once

#include "01_Util/chunk.hpp"

#include <unordered_map>

//	16 KB
#define CHUNK_MAX_SIZE 16000

class archetype_base
{
protected:
	archetype_base() : _chunk(nullptr) {}
public:
	virtual ~archetype_base()
	{
		if (_chunk != nullptr) delete _chunk;
	}

	template<typename T>
	bool is_exist_type()
	{
		return _type_map.count(typeid(T)) > 0;
	}

	template<typename T>
	void input(T*&& arr, size_t length)
	{
		if (is_exist_type<T>() == false) return;
		if (arr == nullptr) return;
		if (_chunk == nullptr) return;

		_chunk->input_data(arr, length, get_element_type_index<T>());
	}

	template<typename T>
	size_t output(T*& arr)
	{
		if (is_exist_type<T>() == false) return 0;
		if (arr == nullptr) return 0;
		if (_chunk == nullptr) return 0;

		return _chunk->output_data(arr, get_element_type_index<T>());
	}



protected:
	template<typename T>
	void set_element_type_index(short element_index)
	{
		_type_map[typeid(T)] = element_index;
	}

	template<typename T>
	short get_element_type_index()
	{
		return _type_map[typeid(T)];
	}


protected:
	chunk* _chunk;
	std::unordered_map<type_info, short> _type_map;
};

template<typename T1>
class archetype : public archetype_base
{
public:
	archetype()
	{
		size_t sarr[1] = { sizeof(T1) };
		size_t total = sarr[0];
		_chunk = new chunk(CHUNK_MAX_SIZE / total, 1, total, sarr);

		set_element_type_index<T1>(0);
	}

};