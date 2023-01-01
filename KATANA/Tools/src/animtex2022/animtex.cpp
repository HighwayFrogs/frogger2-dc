#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include "crc32.h"

// Bring to upper case, and stop at the file extension period, if there is one, then hash it.
unsigned long hash(char* input) {
	unsigned long i = 0;
	while (input[i] != '.' && input[i] != '\0' && input[i] != '\n')
	{
		if (input[i] >= 'a' && input[i] <= 'z')
			input[i] -= 32; // Converts to upper-case.

		i++;
	}
	input[i] = '\0';
	return UpdateCRC(input);
}

void read_next_line(std::ifstream& in, char* line) {
	do
	{
		if (in.eof()) {
			std::cerr << "Reached eof before reading all the data which was expected to be there." << std::endl;
			exit(1);
		}

		in.getline(line, 256, '\n');
	} while (!line[0] && !in.eof());
}

int main( int argc, char **argv )
{
	std::ifstream in;
	std::ofstream out;
	char line[256];
	int len, i;
	unsigned long uid, num, number_of_frames;

	if (argc < 3)
	{
		std::cerr << "Syntax: " << argv[0] << " <output_file> <input_files...>" << std::endl;
	    std::cerr << "This recreation was created by Kneesnap on 12/31/2022." << std::endl;
		exit(1);
	}

	char* outputFile = argv[1];

	// Create output file
	out.open( outputFile, std::ios::out | std::ios::trunc | std::ios::binary, _SH_DENYRW );
	if( !out )
	{
		std::cerr << "Cannot write to file %s" << outputFile << std::endl;
		return -2;
	}

	InitCRCTable();

	// Write file count.
	unsigned long fileCount = argc - 2;
	out.write((char*)&fileCount, sizeof(unsigned long));

	// Write files.
	char* inputFile = argv[2];
	for (i = 2; i < argc; i++)
	{
		char* inputFile = argv[i];

		// Open text file containing user data
		std::cerr << "Parsing " << inputFile << "..." << std::endl;
		in.open(inputFile, std::ios::in, _SH_DENYNO);
		if (!in)
		{
			std::cerr << "Cannot read from file " << inputFile << std::endl;
			return -1;
		}
		in.setf(std::ios::skipws); // Ignore blank lines

		// Read nubmer of frames.
		in >> number_of_frames;
		out.write((char*)&number_of_frames, sizeof(unsigned long));
		std::cerr << " - Frames: " << number_of_frames << std::endl;

		// Write file name hash.
		unsigned long fileNameHash = hash(inputFile);
		out.write((char*)&fileNameHash, sizeof(unsigned long));
		std::cerr << " - Hash: " << fileNameHash << std::endl;

		unsigned long lastWaitTime = 0x80000000;
		for (unsigned long frame = 0; frame < number_of_frames; frame++)
		{
			// Read & write image hash.
			read_next_line(in, &line[0]); // TODO: crashes in here.
			unsigned long imageNameHash = hash(line);
			out.write((char*)&imageNameHash, sizeof(unsigned long));
			std::cerr << "  - Image: '" << line << "'." << std::endl;


			// Read & write 
			read_next_line(in, &line[0]);
			if (!strcmp(line, "\""))
			{
				out.write((char*)&lastWaitTime, sizeof(unsigned long));
			}
			else
			{
				unsigned long waitTime = atoi(line);
				out.write((char*)&waitTime, sizeof(unsigned long));
				lastWaitTime = waitTime;
			}
		}

		in.close();
	}

	std::cerr << "Finished, OK." << std::endl;
	out.close();

	return 0;
}
