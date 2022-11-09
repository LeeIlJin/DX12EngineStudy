#pragma once

class list_node_base
{
protected:
	list_node_base() :_next(nullptr) {}
	virtual ~list_node_base() { if (_next != nullptr) { delete _next; } }

	list_node_base* add_next(list_node_base* n)
	{
		if (_next == nullptr)
		{
			_next = n;
			return this;
		}
		return _next->add_next(n);
	}

	list_node_base* pop_last()
	{
		if (_next == nullptr)
		{
			return nullptr;
		}

		if (_next->_next == nullptr)
		{
			list_node_base* n = _next;
			_next = nullptr;
			return n;
		}

		return _next->pop_last();
	}

	size_t get_count()
	{
		if (_next == nullptr)
			return 1;
		return _next->get_count() + 1;
	}

	bool next_null() { return (_next == nullptr); }
	template<typename T>
	T* next() { return static_cast<T*>(_next); }

	template<typename T>
	T* last() { if (_next == nullptr) { return static_cast<T*>(this); } return _next->last<T>(); }

private:
	list_node_base* _next;
};
