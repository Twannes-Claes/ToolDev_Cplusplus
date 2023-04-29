#include "InputOuptutBlocks.h"

#include <codecvt>

#include <fstream>

#include <RapidJson/document.h>
#include <RapidJson/istreamwrapper.h>

bool InputOuptutBlocks::ReadInputFile(const std::wstring& inputFileName)
{

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

									//when everything is correct emplace new block
									m_Blocks.emplace_back(newBlock);

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
		return false;
	}

	return true;
}

bool InputOuptutBlocks::WriteOuptutFile(const std::wstring& outputFileName)
{

	//create outputfile

	FILE* pOutputFile{};

	_wfopen_s(&pOutputFile, outputFileName.c_str(), L"w+,ccs=UTF-8");

	if (pOutputFile)
	{
		//write away comment
		const wchar_t* text = L" # is the symbol for partial derivative.\n";
		fwrite(text, wcslen(text) * sizeof(wchar_t), 1, pOutputFile);

		//write away the material library
		fwprintf_s(pOutputFile, L"mtllib minecraftMats.mtl\n\n");

		//write away all the different normals
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, 1.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, -1.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 0.0f, 1.0f, 0.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 0.0f, -1.0f, 0.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 1.0f, 0.0f, 0.0f);
		fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n\n", -1.0f, 0.0f, 0.0f);

		//write away all uv coords
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n", 0.f, 0.f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n", 1.f, 0.f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n", 0.f, 1.f);
		fwprintf_s(pOutputFile, L"vt %.2f %.2f\n", 1.f, 1.f);

		//create offset for the indices
		constexpr int indiceOffset{ 8 };

		int currIdx{};

		for (const auto& block : m_Blocks)
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
		return false;
	}

	return true;
}

void InputOuptutBlocks::WriteAwayBlock(FILE* pOutputFile, const Block& block, const int offset)
{
	const Vector3& blockPos = block.position;

	//write away vertices

	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", blockPos.x - 0.5f, blockPos.y - 0.5f, blockPos.z - 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", blockPos.x - 0.5f, blockPos.y - 0.5f, blockPos.z + 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", blockPos.x - 0.5f, blockPos.y + 0.5f, blockPos.z - 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", blockPos.x - 0.5f, blockPos.y + 0.5f, blockPos.z + 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", blockPos.x + 0.5f, blockPos.y - 0.5f, blockPos.z - 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", blockPos.x + 0.5f, blockPos.y - 0.5f, blockPos.z + 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", blockPos.x + 0.5f, blockPos.y + 0.5f, blockPos.z - 0.5f);
	fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", blockPos.x + 0.5f, blockPos.y + 0.5f, blockPos.z + 0.5f);

	//all the info for the faces

	std::vector<WriteFaceInfo> infos{};

	LoadFaceInfo(infos, blockPos, offset);

	auto it{ std::vector<Block>::iterator{} };

	for (const auto& info : infos)
	{
		WriteFaces(pOutputFile, it, info, block.isOpaque);
	}

}

void InputOuptutBlocks::WriteFaces(FILE* pOutputFile, std::vector<Block>::iterator& it, const WriteFaceInfo& info, const bool isOpaque)
{
	//cache the info values
	const Vector3& position = info.position;
	const Vector3& comparePos = info.comparePosition;

	const int& offset = info.offset;

	const int& face = info.face;

	const  int& vX = info.vX;
	const Int2& vY = info.vY;
	const Int2& vZ = info.vZ;

	const  int& uvX = info.uvX;
	const Int2& uvY = info.uvY;
	const Int2& uvZ = info.uvZ;

	//if is not opaque write away the faces

	if (isOpaque == false)
	{
		fwprintf_s(pOutputFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", vX + offset, uvX, face, vY.x + offset, uvY.x, face, vZ.x + offset, uvZ.x, face);
		fwprintf_s(pOutputFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", vX + offset, uvX, face, vY.y + offset, uvY.y, face, vZ.y + offset, uvZ.y, face);
		return;
	}

	//when opaque write away faces when no neighbours found

	it = std::find_if(std::begin(m_Blocks), std::end(m_Blocks), [&](const Block& blockLambda)
	{
		return blockLambda.position.x == position.x + comparePos.x && blockLambda.position.y == position.y + comparePos.y && blockLambda.position.z == position.z + comparePos.z && blockLambda.isOpaque;
	});

	if (it == m_Blocks.end())
	{
		fwprintf_s(pOutputFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", vX + offset, uvX, face, vY.x + offset, uvY.x, face, vZ.x + offset, uvZ.x, face);
		fwprintf_s(pOutputFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", vX + offset, uvX, face, vY.y + offset, uvY.y, face, vZ.y + offset, uvZ.y, face);
	}
}

void InputOuptutBlocks::LoadFaceInfo(std::vector<WriteFaceInfo>& vector, const Vector3& blockPos, const int offset) const
{
	WriteFaceInfo info;

	info.position = blockPos;
	info.offset = offset;

	//faces behind
	info.comparePosition = Vector3{ 0.f,0.f,-1.f };

	info.face = 2;

	info.vX = 1;
	info.vY = Int2{ 7 , 3 };
	info.vZ = Int2{ 5 , 7 };

	info.uvX = 3;
	info.uvY = Int2{ 2 , 1 };
	info.uvZ = Int2{ 4 , 2 };

	vector.emplace_back(info);

	//faces left

	info.comparePosition = Vector3{ -1.f,0.f, 0.f };

	info.face = 6;

	info.vX = 1;
	info.vY = Int2{ 4 , 2 };
	info.vZ = Int2{ 3 , 4 };

	info.uvX = 4;
	info.uvY = Int2{ 1 , 3 };
	info.uvZ = Int2{ 2 , 1 };

	vector.emplace_back(info);

	//faces above

	info.comparePosition = Vector3{ 0.f,1.f, 0.f };

	info.face = 3;

	info.vX = 3;
	info.vY = Int2{ 8 , 4 };
	info.vZ = Int2{ 7 , 8 };

	info.uvX = 3;
	info.uvY = Int2{ 2 , 1 };
	info.uvZ = Int2{ 4 , 2 };

	vector.emplace_back(info);

	//faces right

	info.comparePosition = Vector3{ 1.f, 0.f, 0.f };

	info.face = 5;

	info.vX = 5;
	info.vY = Int2{ 7 , 8 };
	info.vZ = Int2{ 8 , 6 };

	info.uvX = 3;
	info.uvY = Int2{ 1 , 2 };
	info.uvZ = Int2{ 2 , 4 };

	vector.emplace_back(info);

	//faces down

	info.comparePosition = Vector3{ 0.f, -1.f, 0.f };

	info.face = 4;

	info.vX = 1;
	info.vY = Int2{ 5 , 6 };
	info.vZ = Int2{ 6 , 2 };

	info.uvX = 1;
	info.uvY = Int2{ 2 , 4 };
	info.uvZ = Int2{ 4 , 3 };

	vector.emplace_back(info);

	//faces forward

	info.comparePosition = Vector3{ 0.f, 0.f, 1.f };

	info.face = 1;

	info.vX = 2;
	info.vY = Int2{ 6 , 8 };
	info.vZ = Int2{ 8 , 4 };

	info.uvX = 4;
	info.uvY = Int2{ 3 , 1 };
	info.uvZ = Int2{ 1 , 2 };

	vector.emplace_back(info);
}
