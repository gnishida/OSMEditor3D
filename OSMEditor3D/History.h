#pragma once

#include <vector>
#include "RoadGraph.h"

class History {
private:
	int index;
	std::vector<RoadGraph> history;

public:
	History();

	void push(RoadGraph roads);
	RoadGraph undo();
	RoadGraph redo();
};

