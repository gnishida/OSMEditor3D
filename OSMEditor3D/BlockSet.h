#pragma once

#include "VBORenderManager.h"
#include <vector>
#include <QString>
#include <QVector2D>
#include "Block.h"
#include "Parcel.h"

class BlockSet {
public:
	std::vector<Block> blocks;

public:
	BlockSet() {}

	Block& operator[](int index) { return blocks[index]; }
	Block& at(int index) { return blocks.at(index); }
	const Block& operator[](int index) const { return blocks[index]; }
	size_t size() const { return blocks.size(); }
	void clear();
};

