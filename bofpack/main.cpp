#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>

#include "bof_pack.h"

int main(int argc, char* argv[])
{
	if (argc == 1) {
		std::cout << "bofpack -o dst -i integer -s short -b binary file -a \"ascii string\" -w \"wide string\"" << std::endl;
		return 0;
	}

	bof_pack pack;

	std::string outputFile;

	for (int i = 1; i < argc; i += 2) {
		if (argv[i][0] != '-') {
			std::cout << "wrong pattern " << argv[i] << std::endl;
			return 0;
		}

		if (argv[i][1] == 'o') {
			outputFile = argv[i + 1];
		}
		else if (argv[i][1] == 'i') {
			pack.encodeInteger(std::atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 's') {
			pack.encodeShort((short)std::atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'a') {
			pack.encodeString(argv[i + 1]);
		}
		else if (argv[i][1] == 'w') {
			pack.encodeWideString(argv[i + 1]);
		}
		else if (argv[i][1] == 'b') {
			if (pack.encodeBinary(argv[i + 1]) == false) {
				std::cout << "Cannot open " << argv[i + 1] << std::endl;
				return 0;
			}
		}
	}

	std::ofstream output(outputFile, std::ios::binary);

	if (!output) {
		std::cout << "Error opening " << outputFile << std::endl;
		return 0;
	}
	output.write(pack.bof, pack.bof_size);

	output.close();

	std::cout << "file generated. Size " << pack.bof_size << " bytes." << std::endl;
	return 0;
}