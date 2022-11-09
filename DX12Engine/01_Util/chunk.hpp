#pragma once

#include "01_Util/elements_sorted_array.hpp"
#include "01_Util/list_node.hpp"

class chunk_base : protected elements_sorted_array_base, protected list_node_base
{
protected:
	chunk_base(const size_t& array_length, const short& element_length, const size_t& elements_total_size)
		:elements_sorted_array_base(array_length, element_length, elements_total_size), list_node_base() {}
public:
	virtual ~chunk_base() {}

public:
	void input_data(void*& p, const size_t& length, const short& element)
	{
		size_t remain = length;
		size_t nextp = (size_t)p;
		chunk_base* c = this;
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
				c = c->next<chunk_base>();
		}

		delete p;
		p = nullptr;
	}

	size_t output_data(void*& p, const short& element)
	{
		size_t total_length = 0;
		{
			total_length = last<chunk_base>()->get_array_use();
			total_length += ((get_count() - 1) * get_array_use());
			p = malloc(total_length * get_element_size(element));
		}

		size_t nextp = (size_t)p;
		chunk_base* c = this;
		while (true)
		{
			size_t use = c->copyTo((void*)nextp, element);
			nextp += (use * c->get_element_size(element));

			if (c->next_null())
				break;
			
			c = c->next<chunk_base>();
		}

		return total_length;
	}

protected:
	//	새로 만들어진 next를 반환
	virtual chunk_base* add_new_next() = 0;
};




//	함수생성 매크로
#define CHUNK_CONS_1(a) chunk_base(l,1,a){size_t es = new size_t[1]{ a };init_element_sizes(es);}
#define CHUNK_CONS_2(a,b) chunk_base(l,2,a+b){size_t es = new size_t[2]{ a,b };init_element_sizes(es);}
#define CHUNK_CONS_3(a,b,c) chunk_base(l,3,a+b+c){size_t es = new size_t[3]{ a,b,c };init_element_sizes(es);}
#define CHUNK_CONS_4(a,b,c,d) chunk_base(l,4,a+b+c+d){size_t es = new size_t[4]{ a,b,c,d };init_element_sizes(es);}
#define CHUNK_CONS_5(a,b,c,d,e) chunk_base(l,5,a+b+c+d+e){size_t es = new size_t[5]{ a,b,c,d,e };init_element_sizes(es);}
#define CHUNK_CONS_6(a,b,c,d,e,f) chunk_base(l,6,a+b+c+d+e+f){size_t es = new size_t[6]{ a,b,c,d,e,f };init_element_sizes(es);}
#define CHUNK_CONS_7(a,b,c,d,e,f,g) chunk_base(l,7,a+b+c+d+e+f+g){size_t es = new size_t[7]{ a,b,c,d,e,f,g };init_element_sizes(es);}
#define CHUNK_CONS_8(a,b,c,d,e,f,g,h) chunk_base(l,8,a+b+c+d+e+f+g+h){size_t es = new size_t[8]{ a,b,c,d,e,f,g,h };init_element_sizes(es);}
#define CHUNK_CONS_9(a,b,c,d,e,f,g,h,i) chunk_base(l,9,a+b+c+d+e+f+g+h+i){size_t es = new size_t[9]{ a,b,c,d,e,f,g,h,i };init_element_sizes(es);}
#define CHUNK_CONS_10(a,b,c,d,e,f,g,h,i,j) chunk_base(l,10,a+b+c+d+e+f+g+h+i+j){size_t es = new size_t[10]{ a,b,c,d,e,f,g,h,i,j };init_element_sizes(es);}



template<typename T1>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_1(sizeof(T1))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1>(get_array_length()); add_next(n); return n; }

};

template<typename T1, typename T2>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_2(sizeof(T1), sizeof(T2))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2>(get_array_length()); add_next(n); return n; }
};

template<typename T1, typename T2, typename T3>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_3(sizeof(T1), sizeof(T2), sizeof(T3))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2, T3>(get_array_length()); add_next(n); return n; }
};

template<typename T1, typename T2, typename T3, typename T4>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_4(sizeof(T1), sizeof(T2), sizeof(T3), sizeof(T4))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2, T3, T4>(get_array_length()); add_next(n); return n; }
};

template<typename T1, typename T2, typename T3, typename T4, typename T5>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_5(sizeof(T1), sizeof(T2), sizeof(T3), sizeof(T4), sizeof(T5))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2, T3, T4, T5>(get_array_length()); add_next(n); return n; }
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_6(sizeof(T1), sizeof(T2), sizeof(T3), sizeof(T4), sizeof(T5), sizeof(T6))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2, T3, T4, T5, T6>(get_array_length()); add_next(n); return n; }
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_7(sizeof(T1), sizeof(T2), sizeof(T3), sizeof(T4), sizeof(T5), sizeof(T6), sizeof(T7))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2, T3, T4, T5, T6, T7>(get_array_length()); add_next(n); return n; }
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_8(sizeof(T1), sizeof(T2), sizeof(T3), sizeof(T4), sizeof(T5), sizeof(T6), sizeof(T7), sizeof(T8))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2, T3, T4, T5, T6, T7, T8>(get_array_length()); add_next(n); return n; }
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_9(sizeof(T1), sizeof(T2), sizeof(T3), sizeof(T4), sizeof(T5), sizeof(T6), sizeof(T7), sizeof(T8), sizeof(T9))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2, T3, T4, T5, T6, T7, T8, T9>(get_array_length()); add_next(n); return n; }
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
class chunk : public chunk_base
{
public:
	chunk(const size_t& l) :CHUNK_CONS_10(sizeof(T1), sizeof(T2), sizeof(T3), sizeof(T4), sizeof(T5), sizeof(T6), sizeof(T7), sizeof(T8), sizeof(T9), sizeof(T10))
protected:
	virtual chunk_base* add_new_next() { chunk_base* n = new chunk<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(get_array_length()); add_next(n); return n; }
};


//#undef CHUNK_CONS_1
//#undef CHUNK_CONS_2
//#undef CHUNK_CONS_3
//#undef CHUNK_CONS_4
//#undef CHUNK_CONS_5
//#undef CHUNK_CONS_6
//#undef CHUNK_CONS_7
//#undef CHUNK_CONS_8
//#undef CHUNK_CONS_9
//#undef CHUNK_CONS_10
//#undef CHUNK_ADD_FUNCTION
