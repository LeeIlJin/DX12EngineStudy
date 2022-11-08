#pragma once

class list_node_base
{
protected:
	list_node_base() :next(nullptr) {}
	virtual ~list_node_base() {}

	virtual list_node_base* add_next(list_node_base* n)
	{
		if (next == nullptr)
		{
			next = n;
			return this;
		}
		return next->add_next(n);
	}

	virtual list_node_base* pop_last()
	{
		if (next == nullptr)
		{
			return nullptr;
		}

		if (next->next == nullptr)
		{
			list_node_base* n = next;
			next = nullptr;
			return n;
		}

		return next->pop_last();
	}

protected:
	list_node_base* next;
};
