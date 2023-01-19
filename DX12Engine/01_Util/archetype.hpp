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
	void input(T*& arr, size_t length)
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

#pragma region SetArchetypeFunctions

	template<typename T1>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
	}

	template<typename T1, typename T2>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
	}

	template<typename T1, typename T2, typename T3>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
	}

	template<typename T1, typename T2, typename T3, typename T4>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
		_typehash_map[typeid(T5).hash_code()] = 4;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
		_typehash_map[typeid(T5).hash_code()] = 4;
		_typehash_map[typeid(T6).hash_code()] = 5;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
		_typehash_map[typeid(T5).hash_code()] = 4;
		_typehash_map[typeid(T6).hash_code()] = 5;
		_typehash_map[typeid(T7).hash_code()] = 6;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
		_typehash_map[typeid(T5).hash_code()] = 4;
		_typehash_map[typeid(T6).hash_code()] = 5;
		_typehash_map[typeid(T7).hash_code()] = 6;
		_typehash_map[typeid(T8).hash_code()] = 7;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
		_typehash_map[typeid(T5).hash_code()] = 4;
		_typehash_map[typeid(T6).hash_code()] = 5;
		_typehash_map[typeid(T7).hash_code()] = 6;
		_typehash_map[typeid(T8).hash_code()] = 7;
		_typehash_map[typeid(T9).hash_code()] = 8;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
		_typehash_map[typeid(T5).hash_code()] = 4;
		_typehash_map[typeid(T6).hash_code()] = 5;
		_typehash_map[typeid(T7).hash_code()] = 6;
		_typehash_map[typeid(T8).hash_code()] = 7;
		_typehash_map[typeid(T9).hash_code()] = 8;
		_typehash_map[typeid(T10).hash_code()] = 9;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
		_typehash_map[typeid(T5).hash_code()] = 4;
		_typehash_map[typeid(T6).hash_code()] = 5;
		_typehash_map[typeid(T7).hash_code()] = 6;
		_typehash_map[typeid(T8).hash_code()] = 7;
		_typehash_map[typeid(T9).hash_code()] = 8;
		_typehash_map[typeid(T10).hash_code()] = 9;
		_typehash_map[typeid(T11).hash_code()] = 10;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
	void SetArchetype()
	{
		_typehash_map[typeid(T1).hash_code()] = 0;
		_typehash_map[typeid(T2).hash_code()] = 1;
		_typehash_map[typeid(T3).hash_code()] = 2;
		_typehash_map[typeid(T4).hash_code()] = 3;
		_typehash_map[typeid(T5).hash_code()] = 4;
		_typehash_map[typeid(T6).hash_code()] = 5;
		_typehash_map[typeid(T7).hash_code()] = 6;
		_typehash_map[typeid(T8).hash_code()] = 7;
		_typehash_map[typeid(T9).hash_code()] = 8;
		_typehash_map[typeid(T10).hash_code()] = 9;
		_typehash_map[typeid(T11).hash_code()] = 10;
		_typehash_map[typeid(T12).hash_code()] = 11;
	}


//SetArchetypeFunctions
#pragma endregion


protected:
	template<typename T>
	void set_element_type_index(short element_index)
	{
		_type_map[typeid(T)] = element_index;
	}

	template<typename T>
	short get_element_type_index()
	{
		for (short i = 0; i < _chunk->get_element_length(); i++)
			if (_hash_array[i] == typeid(T).hash_code())
				return i;

		return -1;
	}

	template<typename T>
	static size_t hash() { return typeid(T).hash_code(); }




protected:
	chunk* _chunk;
	size_t* _hash_array;
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