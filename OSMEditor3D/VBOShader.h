#pragma once

#include "glew.h"
#include "VBOUtil.h"
#include "qfile.h"
#include <vector>
#include <map>
#include <QHash>


class Shader{

public: 
	static std::vector<uint> programs;
	static std::vector<uint> vss;
	static std::vector<uint> fragments;

	static uint initShader(const QString& vertFileName, const QString& fragFileName);
	static void cleanShaders();

};

