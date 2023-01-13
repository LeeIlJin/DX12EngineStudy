#pragma once

#include "01_Util/elements_sorted_array.hpp"
#include "01_Util/list_node.hpp"

/* 요렇게 테스트 해서 돌아감! 잘! 돌아감!
	chunk* a = nullptr;
    {
        size_t arr[2]{ 4,4 };
        a = new chunk(3, 2, 8, arr);
    }
    
    {
        int* varr = new int[7] { 1, 2, 3, 4, 5, 6, 7 };
        int *varr2 = new int[7]{ 39,20,87,31,65,37,99 };

        a->input_data(varr, 7, 0);
        a->input_data(varr2, 7, 1);

        if (varr == nullptr)
            int poe = 0; // 디버깅용
    }

    {
        int* varr = nullptr;
        int* varr2 = nullptr;

        size_t i = a->output_data(varr, 0);
        size_t i2 = a->output_data(varr2, 1);

        if (varr != nullptr)
        {
            int p1 = varr[0];
            int p2 = varr[1];
            int p3 = varr[2];
            int p4 = varr[3];
            int p5 = varr[4];
            int p6 = varr[5];
            int p7 = varr[6];

            int p11 = varr2[0];
            int p22 = varr2[1];
            int p33 = varr2[2];
            int p44 = varr2[3];
            int p55 = varr2[4];
            int p66 = varr2[5];
            int p77 = varr2[6];

            int poe = 0; // 디버깅해서 값 확인용
        }

        if (varr)
            delete varr;
        if (varr2)
            delete varr2;
    }

    delete a;
*/



class chunk : protected elements_sorted_array_base, protected list_node_base
{
public:
	chunk(const size_t& array_length, const short& element_length, const size_t& elements_total_size, size_t * element_size_array)
		:elements_sorted_array_base(array_length, element_length, elements_total_size), list_node_base()
	{
		init_element_sizes(element_size_array);
	}
	virtual ~chunk() {}

public:
	template<typename T>
	void input_data(T&& p, const size_t& length, const short& element)
	{
		size_t remain = length;
		size_t nextp = (size_t)p;
		chunk* c = this;
		while (true)
		{
			size_t use = c->copyFrom((void*)nextp, remain, element);
			nextp += (use * c->get_element_size(element));
			remain -= use;

			if (remain == 0)
				break;

			if (c->next_null())
				c = c->add_new_next();
			else
				c = static_cast<chunk*>(c->next());
		}

		delete p;
		p = nullptr;
	}

	template<typename T>
	size_t output_data(T*& p, const short& element)
	{
		size_t total_length = 0;
		{
			total_length = static_cast<chunk*>(last())->get_array_use();
			total_length += ((get_count() - 1) * get_array_use());
			p = (T*)malloc(total_length * get_element_size(element));
		}

		size_t nextp = (size_t)p;
		chunk* c = this;
		while (true)
		{
			size_t use = c->copyTo((void*)nextp, element);
			nextp += (use * c->get_element_size(element));

			if (c->next_null())
				break;
			
			c = static_cast<chunk*>(c->next());
		}

		return total_length;
	}

	size_t get_length()
	{
		size_t total_length = static_cast<chunk*>(last())->get_array_use();
		total_length += ((get_count() - 1) * get_array_use());
		return total_length;
	}

	template<typename T>
	void add_data(T&& p, const size_t& length, const short& element)
	{
		
	}

protected:
	//	새로 만들어진 next를 반환
	virtual chunk* add_new_next()
	{
		size_t* arr = get_element_size_array();
		chunk* new_chunk = new chunk(get_array_length(), get_element_length(), get_elements_total_size(), std::move(arr));
		add_next(new_chunk);
		return new_chunk;
	}
};

