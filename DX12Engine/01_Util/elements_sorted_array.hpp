#pragma once

#include <memory>

class elements_sorted_array_base
{
protected:
	elements_sorted_array_base(const size_t& array_length, const short& element_length, const size_t& elements_total_size)
		:array_length(array_length), element_length(element_length), elements_total_size(elements_total_size), element_sizes(nullptr), array_use(0)
	{
		element_sizes = new size_t[element_length];
		buffer = malloc(array_length * elements_total_size);
		buffer_i = (size_t)buffer;
	}
	virtual ~elements_sorted_array_base()
	{
		if (buffer != nullptr) { delete buffer; }
		if (element_sizes != nullptr) { delete element_sizes; }
	}

protected:
	inline void init_element_sizes(size_t* p) { memcpy(element_sizes, p, sizeof(size_t) * element_length); }

	inline size_t get_element_array_point(const short& element , const size_t offset_length = 0)
	{
		size_t pass = 0;
		for (short i = 0; i < element; i++)
		{
			pass += element_sizes[i] * array_length;
		}
		return buffer_i + pass + (offset_length * element_sizes[element]);
	}

	inline size_t* get_element_size_array()
	{
		return element_sizes;
	}

public:
	inline size_t get_array_length() { return array_length; }
	inline size_t get_array_use() { return array_use; }
	inline size_t get_element_length() { return element_length; }
	inline size_t get_element_size(const short& element) { return element_sizes[element]; }
	inline size_t get_elements_total_size() { return elements_total_size; }

	//	p �� element �ε����� �´� �迭�� �����ϰ� �� �迭�� ���̸� ��ȯ�Ѵ�.
	size_t copyTo(void* p, const short& element)
	{
		memcpy(p, (void*)(get_element_array_point(element)), element_sizes[element] * array_use);
		return array_use;
	}

	//	p �� element �ε����� �´� �迭�� length ���� ��ŭ �����ŵ�ϴ�.
	//	���� �ش� �迭�� array_length �� �� �۴ٸ� ����� ���� ���� ��ȯ�Ѵ�.(array_length ��ȯ)
	//	length�� �� ������... (length ��ȯ)
	size_t copyFrom(const void* p, const size_t& length, const short& element)
	{
		size_t min = ((array_length < length) ? array_length : length);
		array_use = min;
		memcpy((void*)get_element_array_point(element), p, element_sizes[element] * min);
		return min;
	}

	//	p_array [ element_length ] [ length ]
	//	p_array (element �� ȣȯ, 2�� �迭) �� ��� element �迭�� ������� length ���� ��ŭ �����ŵ�ϴ�.
	//	���� �ش� �迭�� ���� ������ �� �۴ٸ� ����� ���� ���� ��ȯ�Ѵ�.(array_length - array_use ��ȯ)
	//	length�� �� ������... (length ��ȯ)
	size_t addFrom(const void** p_array, const size_t& length, const size_t begin = 0)
	{
		size_t array_remain = array_length - array_use;
		size_t min = ((array_remain < length) ? array_remain : length);
		for (short i = 0; i < element_length; i++)
			memcpy((void*)get_element_array_point(i,array_use), (void*)((size_t)p_array[i] + begin * element_sizes[i]), element_sizes[i] * min);
		array_use = min;
		return min;
	}

private:
	const size_t array_length;
	size_t array_use;

	void* buffer;
	size_t buffer_i;

	const short element_length;
	const size_t elements_total_size;
	size_t* element_sizes;
};