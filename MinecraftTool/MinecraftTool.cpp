#include <codecvt>

#include "rapidjson.h"
#include "document.h"
#include "istreamwrapper.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

//#include "shellapi.h"

//#include "vld.h"

struct Vector3
{
	float x{}, y{}, z{};
};

struct Block
{
	Vector3 position{};

	std::wstring layer{};

	bool isOpaque{};
};


std::vector<Block> g_Blocks;

void Help();
void WriteAwayBlock(FILE* pOutputFile, const Block& block, const int offset);
void WriteNonDuplicateFaces(FILE* pOutputFile, std::vector<Block>::iterator& it, const Vector3& position, const Vector3 comparePos, const int offset, const int startFace, const int secondFace, const std::pair<int, int>& firstPair, const std::pair<int, int>& secondPair);


int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{

	std::wstring inputFileName{};
	std::wstring outputFileName{};

	const std::wstring dotJson{L".json"};
	const std::wstring dotObj{L".obj"};

	//check for right arguments
	if(argc != 3 && argc != 5)
	{
		Help();
		return -1;
	}

	for(int i = 1; i < argc; i += 2)
	{

		std::wstring argument = argv[i];

		//check if argument is not size 2
		if(argument.size() != 2)
		{
			Help();
			return -1;
		}

		//switch over the -i / -o
		switch (argument[1])
		{
			case 'i':
			{
				inputFileName = argv[i + 1];

				//check if file extension is right
				if (inputFileName.size() <= 5 || inputFileName.substr(inputFileName.size() - 5) != dotJson)
				{
					wprintf_s(L"inputFile needs to have at least one character and must end with .json\n");
					Help();
					return -1;
				}
			}
			break;
			case 'o':
			{
				outputFileName = argv[i + 1];

				//check if file extension is right
				if (outputFileName.size() <= 4 || outputFileName.substr(outputFileName.size() - 4) != dotObj)
				{
					wprintf_s(L"outputFile needs to have at least one character and must end with .obj\n");
					Help();
					return -1;
				}
			}
			break;
			default:
			{
				//if wrong send help
				Help();
				return -1;
			}
		}


	}

	//if no output defined, define output
	if(outputFileName.empty())
	{
		outputFileName = inputFileName;

		//replace .json with .obj
		outputFileName.replace(outputFileName.size() - 5, 5, dotObj);
	}

	//open input file
	if (std::ifstream inputStream{ inputFileName })
	{

		using namespace rapidjson;

		//making variables for json reader
		IStreamWrapper isw{ inputStream };
		Document sceneDoc;
		sceneDoc.ParseStream(isw);

		if (sceneDoc.IsArray())
		{
			//converter for the getstring
			//https://stackoverflow.com/a/73412919
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> UTF_Converter;

			for (auto it = sceneDoc.Begin(); it != sceneDoc.End(); ++it)
			{
				//make new block
				Block newBlock{};

				const Value& blockInfo{ *it };

				//check if variables are correct
				if (blockInfo.HasMember("layer") && blockInfo.HasMember("opaque") && blockInfo.HasMember("positions"))
				{

					//fill in opaque and layer
					const Value& opaque{ blockInfo["opaque"] };

					if (opaque.IsBool()) newBlock.isOpaque = opaque.GetBool();

					const Value& layer{ blockInfo["layer"] };

					if (layer.IsString()) newBlock.layer = UTF_Converter.from_bytes(layer.GetString());

					const Value& positions{ blockInfo["positions"] };

					if (positions.IsArray())
					{

						for (auto posIt = positions.Begin(); posIt != positions.End(); ++posIt)
						{
							const Value& currentPos{ *posIt };

							if (currentPos.IsArray() && currentPos.Size() == 3)
							{
								const Value& x = currentPos[1];
								const Value& y = currentPos[2];
								const Value& z = currentPos[0];

								if (x.IsInt() && y.IsInt() && z.IsInt())
								{
									//fill in new pos
									Vector3 newPos{};

									newPos.x = static_cast<float>(x.GetInt());
									newPos.y = static_cast<float>(y.GetInt());
									newPos.z = static_cast<float>(z.GetInt());

									newBlock.position = newPos;

									//wprintf_s(L"%.4f %.4f %.4f\n", newPos.x, newPos.y, newPos.z);

									//when everything is correct emplace new block
									g_Blocks.emplace_back(newBlock);

								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		//if file couldnt open return
		wprintf_s(L"Couldn't open inputfile\n");
		wprintf_s(L"Make sure you either have the file or named it wrong\n");
		Help();
		return -1;
	}

	//create outputfile

	FILE* pOutputFile{};

	_wfopen_s(&pOutputFile, outputFileName.c_str(), L"w+,ccs=UTF-8");

	if(pOutputFile)
	{

		//create offset for the indices
		constexpr int indiceOffset{ 8 };

		//write away comment
		const wchar_t* text = L" # is the symbol for partial derivative.\n";
		fwrite(text, wcslen(text) * sizeof(wchar_t), 1, pOutputFile);

		//write away the material library
		fwprintf_s(pOutputFile, L"mtllib minecraftMats.mtl\n\n");

		//write away all the different normals
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n",   0.0f, 0.0f, 1.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n",   0.0f, 0.0f, -1.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n",   0.0f, 1.0f, 0.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n",   0.0f, -1.0f, 0.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n",   1.0f, 0.0f, 0.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n\n", -1.0f, 0.0f, 0.0f);

		//write away all uv coords
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",    0.25f,   0.5f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",     0.5f,   0.5f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",     0.5f,  0.75f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",    0.25f,  0.75f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",    0.75f,   0.5f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",    0.75f,  0.75f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",	    1.f,   0.5f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",	    1.f,  0.75f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",	  0.25f,    1.f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",	   0.5f,    1.f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",	    0.f,  0.75f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",      0.f,   0.5f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n",    0.25f,  0.25f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n\n",    0.5f,  0.25f);

		int currIdx{};

		for(const auto& block : g_Blocks)
		{
			const Block& blockInfo{ block };

			//print the layer
			fwprintf_s(pOutputFile, L"usemtl %ls\n", block.layer.c_str());

			//print the block
			WriteAwayBlock(pOutputFile, blockInfo, currIdx);

			//print enter after done
			fwprintf_s(pOutputFile, L"\n");

			//offset the index
			currIdx += indiceOffset;
		}

		fclose(pOutputFile);

		wprintf_s(L"\nOperation succeeded\n");

		//todo make this work
		//ShellExecute(NULL, L"open", L"explorer.exe", outputFileName.c_str(), NULL, 10);
	}
	else
	{
		wprintf_s(L"\nCouldn't open inputfile\n");
		Help();
		return -1;
	}

	return 0;
	
}

void WriteAwayBlock(FILE* pOutputFile, const Block& block, const int offset)
{
	const Vector3& position = block.position;

	//write away vertices
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", position.x - 0.5f, position.y - 0.5f, position.z - 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", position.x - 0.5f, position.y - 0.5f, position.z + 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", position.x - 0.5f, position.y + 0.5f, position.z - 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", position.x - 0.5f, position.y + 0.5f, position.z + 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", position.x + 0.5f, position.y - 0.5f, position.z - 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", position.x + 0.5f, position.y - 0.5f, position.z + 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", position.x + 0.5f, position.y + 0.5f, position.z - 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", position.x + 0.5f, position.y + 0.5f, position.z + 0.5f);


	if(block.isOpaque == false)
	{
		//write away all faces
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 2, 7 + offset, 2, 5 + offset, 2);
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 2, 3 + offset, 2, 7 + offset, 2);

		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 6, 4 + offset, 6, 3 + offset, 6);
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 6, 2 + offset, 6, 4 + offset, 6);

		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 3 + offset, 3, 8 + offset, 3, 7 + offset, 3);
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 3 + offset, 3, 4 + offset, 3, 8 + offset, 3);

		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 5 + offset, 5, 7 + offset, 5, 8 + offset, 5);
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 5 + offset, 5, 8 + offset, 5, 6 + offset, 5);

		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 4, 5 + offset, 4, 6 + offset, 4);
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 4, 6 + offset, 4, 2 + offset, 4);

		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 2 + offset, 1, 6 + offset, 1, 8 + offset, 1);
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 2 + offset, 1, 8 + offset, 1, 4 + offset, 1);
	}
	else
	{
		//make iterator
		auto it{ std::vector<Block>::iterator{} };

		//check faces behind
		WriteNonDuplicateFaces(pOutputFile, it, position, Vector3{ 0.f,0.f,-1.f }  , offset, 1, 2, std::make_pair(7, 3), std::make_pair(5, 7));

		//check faces left
		WriteNonDuplicateFaces(pOutputFile, it, position, Vector3{ -1.f,0.f, 0.f}  , offset, 1, 6, std::make_pair(4, 2), std::make_pair(3, 4));

		//check faces above
		WriteNonDuplicateFaces(pOutputFile, it, position, Vector3{ 0.f,1.f, 0.f }  , offset, 3, 3, std::make_pair(8, 4), std::make_pair(7, 8));

		//check faces right
		WriteNonDuplicateFaces(pOutputFile, it, position, Vector3{ 1.f,0.f, 0.f }  , offset, 5, 5, std::make_pair(7, 8), std::make_pair(8, 6));

		//check faces down
		WriteNonDuplicateFaces(pOutputFile, it, position, Vector3{ 0.f,-1.f, 0.f } , offset, 1, 4, std::make_pair(5, 6), std::make_pair(6, 2));

		//check faces forward
		WriteNonDuplicateFaces(pOutputFile, it, position, Vector3{ 0.f,0.f, 1.f }  , offset, 2, 1, std::make_pair(6, 8), std::make_pair(8, 4));
	}

}

void WriteNonDuplicateFaces(FILE* pOutputFile, std::vector<Block>::iterator& it, const Vector3& position, const Vector3 comparePos, const int offset, const int startFace, const int secondFace, const std::pair<int, int>& firstPair, const std::pair<int, int>& secondPair)
{
	it = std::find_if(begin(g_Blocks), end(g_Blocks), [&](const Block& blockLambda)
	{
		return blockLambda.position.x == position.x + comparePos.x && blockLambda.position.y == position.y + comparePos.y && blockLambda.position.z == position.z + comparePos.z && blockLambda.isOpaque;
	});

	if (it == g_Blocks.end())
	{
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", startFace + offset, secondFace, firstPair.first  + offset, secondFace,  secondPair.first + offset, secondFace);
		fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", startFace + offset, secondFace, firstPair.second + offset, secondFace, secondPair.second + offset, secondFace);
	}
}

void Help()
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

