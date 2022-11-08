#pragma once

#include <memory>

class elements_sorted_array_base
{
protected:
	elements_sorted_array_base(const size_t& array_length, const short& element_length, const size_t& elements_total_size)
		:array_length(array_length), element_length(element_length), element_sizes(nullptr), array_use(0)
	{
		buffer = malloc(array_length * elements_total_size);
		buffer_i = (size_t)buffer;
	}
	virtual ~elements_sorted_array_base()
	{
		if (buffer != nullptr) { delete buffer; }
		if (element_sizes != nullptr) { delete element_sizes; }
	}

protected:
	inline void init_element_sizes(size_t* p) { element_sizes = p; }

	inline size_t get_element_array_point(const short& element)
	{
		size_t pass = 0;
		for (short i = 0; i < element; i++)
		{
			pass += element_sizes[i] * array_length;
		}
		return buffer_i + pass;
	}

public:
	inline size_t get_array_length() { return array_length; }
	inline size_t get_array_use() { return array_use; }
	inline size_t get_element_length() { return element_length; }
	inline size_t get_elemgnt_size(const short& element) { return element_sizes[element]; }

	//	p 에 element 인덱스에 맞는 배열을 복사하고 그 배열의 길이를 반환한다.
	inline size_t copyTo(void* p, const short& element)
	{
		memcpy(p, (void*)(get_element_array_point(element)), element_sizes[element] * array_use);
		return array_use;
	}

	//	p 를 element 인덱스에 맞는 배열에 length 길이 만큼 복사시킵니다.
	//	만약 해당 배열의 array_length 가 더 작다면 복사하지 못한 길이 값을 반환한다.
	inline size_t copyFrom(const void* p, const short& element, const size_t& length)
	{
		size_t min = ((array_length < length) ? array_length : length);
		array_use = min;
		memcpy((void*)get_element_array_point(element), p, element_sizes[element] * min);
		return length - min;
	}

private:
	const size_t array_length;
	size_t array_use;

	void* buffer;
	size_t buffer_i;

	const short element_length;
	size_t* element_sizes;
};