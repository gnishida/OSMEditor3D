#include "History.h"

History::History() {
	index = 0;
}

void History::push(RoadGraph roads) {
	// remove the index-th element and their after
	history.resize(index);

	// add history
	history.push_back(roads.clone());
	index++;
}

RoadGraph History::undo() {
	if (index <= 0) throw "No history.";

	// return the previous state
	return history[--index].clone();
}

RoadGraph History::redo() {
	if (index >= history.size() - 1) throw "No history.";

	// return the next state
	return history[++index].clone();
}

