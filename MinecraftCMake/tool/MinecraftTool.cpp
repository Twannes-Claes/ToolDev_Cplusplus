#include "InputOuptutBlocks.h"

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

	const std::unique_ptr<InputOuptutBlocks> pInputOutputBlocks = std::make_unique<class InputOuptutBlocks>();

	const bool inputCheck = pInputOutputBlocks->ReadInputFile(inputFileName);

	const bool outputCheck = pInputOutputBlocks->WriteOuptutFile(outputFileName);

	if (!inputCheck || !outputCheck) return -1;

	return 0;
}



