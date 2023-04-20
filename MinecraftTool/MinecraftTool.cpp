#include "rapidjson.h"
#include "document.h"
#include "istreamwrapper.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "vld.h"

void FirstPartLes1();
void MinecraftJSON();

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{

	FILE* pOutputFile = nullptr;

	_wfopen_s(&pOutputFile, L"sceneminecraft_zelf.obj", L"w+,ccs=UTF-8");

	const wchar_t* text = L" # is the symbol for partial derivative.\n";
	fwrite(text, wcslen(text) * sizeof(wchar_t), 1, pOutputFile);

	fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, 1.0f);
	fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, -1.0f);
	fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 0.0f, 1.0f, 0.0f);
	fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 0.0f, -1.0f, 0.0f);
	fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", 1.0f, 0.0f, 0.0f);
	fwprintf_s(pOutputFile, L"vn %.4f %.4f %.4f\n", -1.0f, 0.0f, 0.0f);

	if(std::ifstream inputStream{"minecraftscene.json"})
	{
		using namespace rapidjson;
		IStreamWrapper isw{ inputStream };
		Document sceneDoc;
		sceneDoc.ParseStream(isw);

		if(sceneDoc.IsArray())
		{
			int cubeIdx{};

			for(auto it = sceneDoc.Begin(); it != sceneDoc.End(); ++it)
			{
				const Value& cube = *it;

				if(cube.HasMember("layer") && cube.HasMember("opaque") && cube.HasMember("positions"))
				{
					const Value& pos = cube["positions"]; 

					int posIdx{};

					for (auto posIt = pos.Begin(); posIt != pos.End(); ++posIt)
					{
						const Value& currentPos = pos[posIdx];

						if (pos.IsArray() && currentPos.Size() == 3)
						{

							const Value& xVal = currentPos[1];
							const Value& yVal = currentPos[2];
							const Value& zVal = currentPos[0];

							if ((xVal.IsInt() && yVal.IsInt() && zVal.IsInt()) == false) return 0;

							const float x = xVal.GetFloat();
							const float y = yVal.GetFloat();
							const float z = zVal.GetFloat();

							fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", x, y, z);
							fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", x, y, z + 1.0f);
							fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", x, y + 1.0, z);
							fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", x, y + 1.0, z + 1.0f);
							fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", x + 1.0, y, z);
							fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", x + 1.0, y, z + 1.0f);
							fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", x + 1.0, y + 1.0, z);
							fwprintf_s(pOutputFile, L"v %.4f %.4f %.4f\n", x + 1.0, y + 1.0, z + 1.0f);

							const int offset = cubeIdx * 8;

							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 2, 7 + offset, 2, 5 + offset, 2);
							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 2, 3 + offset, 2, 7 + offset, 2);

							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1+ offset, 6, 4 + offset, 6, 3 + offset, 6);
							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1+ offset, 6, 2 + offset, 6, 4 + offset, 6);

							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 3 + offset, 3, 8 + offset, 3, 7 + offset, 3);
							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 3 + offset, 3, 4 + offset, 3, 8 + offset, 3);

							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 5 + offset, 5, 7 + offset, 5, 8 + offset, 5);
							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 5 + offset, 5, 8 + offset, 5, 6 + offset, 5);

							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 4, 5 + offset, 4, 6 + offset, 4);
							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 4, 6 + offset, 4, 2 + offset, 4);

							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 2 + offset, 1, 6 + offset, 1, 8 + offset, 1);
							fwprintf_s(pOutputFile, L"f %d//%d %d//%d %d//%d\n", 2 + offset, 1, 8 + offset, 1, 4 + offset, 1);

							++cubeIdx;
							++posIdx;
						}
					}

				}

			}
		}

		fclose(pOutputFile);
	}

	//todo add positions to vertices

	//eerst alle vertices wegschrijven dan correcte indices wegschrijven

	//lijn beginnen met "g" Cube0 --> dan worden alle indices gerest

	//FILE* pOFile = nullptr;
	//
	//_wfopen_s(&pOFile, L"scene.obj", L"w+,ccs=UTF-8");
	//
	//if (pOFile != nullptr) 
	//{
	//	// it was possible to create the file for writing.
	//	const wchar_t* text = L" # is the symbol for partial derivative.\n";
	//	fwrite(text, wcslen(text) * sizeof(wchar_t), 1, pOFile);
	//
	//	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0, 0.0, 0.0f);
	//	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0, 0.0, 1.0f);
	//	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0, 1.0, 0.0f);
	//	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0, 1.0, 1.0f);
	//	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0, 0.0, 0.0f);
	//	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0, 0.0, 1.0f);
	//	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0, 1.0, 0.0f);
	//	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0, 1.0, 1.0f);
	//
	//	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, 1.0f);
	//	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, -1.0f);
	//	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 1.0f, 0.0f);
	//	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, -1.0f, 0.0f);
	//	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 1.0f, 0.0f, 0.0f);
	//	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", -1.0f, 0.0f, 0.0f);
	//
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1, 2, 7, 2, 5, 2);
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1, 2, 3, 2, 7, 2);
	//
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1, 6, 4, 6, 3, 6);
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1, 6, 2, 6, 4, 6);
	//
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 3, 3, 8, 3, 7, 3);
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 3, 3, 4, 3, 8, 3);
	//
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 5, 5, 7, 5, 8, 5);
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 5, 5, 8, 5, 6, 5);
	//
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1, 4, 5, 4, 6, 4);
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1, 4, 6, 4, 2, 4);
	//
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 2, 1, 6, 1, 8, 1);
	//	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 2, 1, 8, 1, 4, 1);
	//
	//	fclose(pOFile);
	//	return 0;
	//}
	//else 
	//{
	//	return -1;
	//}
}


void FirstPartLes1()
{
	// be careful not to use json in the file itself
	const char* jsonContents = R"json( [ {"loc": [10, 15, 0] }, {"loc": [17, 21, 1] }, {"loc": [41, 33, 2] } ] )json";

	rapidjson::Document jsonDoc;
	jsonDoc.Parse(jsonContents);

	using rapidjson::Value;

	if (!jsonDoc.IsArray()) return;

	for (Value::ConstValueIterator itr = jsonDoc.Begin(); itr != jsonDoc.End(); ++itr)
	{
		const Value& pos = *itr;
		const Value& loc = pos["loc"];

		if (loc.IsArray() && loc.Size() == 3)
		{
			const Value& xVal = loc[0];
			const Value& yVal = loc[1];
			const Value& zVal = loc[2];

			if ((xVal.IsInt() && yVal.IsInt() && zVal.IsInt()) == false) return;

			printf("\n{%d, %d, %d}\n", xVal.GetInt(), yVal.GetInt(), zVal.GetInt());
		}
	}

	const wchar_t* currentWorkingDir = _wgetcwd(nullptr, 0);

	if (currentWorkingDir != nullptr)
	{
		std::cout << "\n";
		const std::wstring current_working_dir(currentWorkingDir);
		std::wcout << current_working_dir << std::endl;
		std::cout << "\n";
	}
}

void MinecraftJSON()
{



}
