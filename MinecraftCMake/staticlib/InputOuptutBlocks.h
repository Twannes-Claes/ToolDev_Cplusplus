#pragma once

#include "Globals.h"

class InputOuptutBlocks
{

public:

	bool ReadInputFile(const std::wstring& inputFileName);

	bool WriteOuptutFile(const std::wstring& outputFileName);

private:

	void WriteAwayBlock(FILE* pOutputFile, const Block& block, const int offset);

	void WriteFaces(FILE* pOutputFile, std::vector<Block>::iterator& it, const WriteFaceInfo& info, const bool isOpaque);

	void LoadFaceInfo(std::vector<WriteFaceInfo>& vector, const Vector3& blockPos, const int offset) const;

	std::vector<Block> m_Blocks;
};

