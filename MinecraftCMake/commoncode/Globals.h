#pragma once
#include <sstream>
#include <string>
#include <vector>

struct Vector3
{
	float x{}, y{}, z{};
};

struct Int2
{
	int x{}, y{};
};

struct Block
{
	Vector3 position{};

	std::wstring layer{};

	bool isOpaque{};
};


struct WriteFaceInfo
{
	Vector3 position{};
	Vector3 comparePosition{};

	int offset{};

	int face{};

	int vX{};
	Int2 vY{};
	Int2 vZ{};

	int uvX{};
	Int2 uvY{};
	Int2 uvZ{};
};

inline void Help()
{
	//todo fix that you pass an extra message

	//todo fix the order of writing dirt then all dirt ones

	std::wstringstream wss;

	wss << L"\nWrong arguments\n";

	wss << L"\nPossible commands:\n";

	wss << L"\nMinecraftTool -i inputfile.json\n";

	wss << L"MinecraftTool -i inputfile.json -o outputfile.obj\n\n";

	wprintf_s(wss.str().c_str());
}
