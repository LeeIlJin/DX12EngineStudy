#pragma once

#include "01_Util/elements_sorted_array.h"
#include "01_Util/list_node.h"

class chunk_base : protected elements_sorted_array_base, public list_node_base
{
protected:
	chunk_base();
	virtual ~chunk_base();

public:


};
