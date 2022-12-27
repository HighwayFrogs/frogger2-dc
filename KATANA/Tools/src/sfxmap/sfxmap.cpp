#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include "crc32.h"

int main( int argc, char **argv )
{
	std::ifstream in;
	std::ofstream out;
	char actor[32], wav[32];
	int len, i;
	unsigned long uid, num;

	if (argc < 3)
	{
		std::cerr << "Syntax: " << argv[0] << " <input> <output>" << std::endl;
		exit(1);
	}

	char* inputFile = argv[1];
	char* outputFile = argv[2];

	// Open text file containing user data
	in.open(inputFile, std::ios::in, _SH_DENYNO );
	if( !in )
	{
		std::cerr << "Cannot read from file " << inputFile << std::endl;
		return -1;
	}
	in.setf(std::ios::skipws); // Ignore blank lines

	// Create output file
	out.open( outputFile, std::ios::out | std::ios::trunc | std::ios::binary, _SH_DENYRW );
	if( !out )
	{
		std::cerr << "Cannot write to file %s" << outputFile << std::endl;
		return -2;
	}

	InitCRCTable();

	std::cerr << "Files OK, parsing..." << std::endl;

	// Now read the data and output to a nice friendly (to a programmer) binary file
	in.getline( actor, 32, '\n' );
	while( !(in.eof()) )
	{
		std::cerr << actor << std::endl;

		// Name of the actor - convert to uid and write to file
		if( !strcmp(actor, "end") )
			break; // Break if end string found

		// Type of actor (player, multiplayer, enemy etc
		in >> num;

		// If not tile based, get actor uid and write
		if( num != 4 )
		{
			uid = UpdateCRC(actor);
			out.write((char *)&uid, sizeof(unsigned long));
		}
		else // Write tile number instead
		{
			int tile = atoi(actor);
			out.write((char *)&tile, sizeof(unsigned long));
		}

		std::cerr << '[';
		// output integer (starting with type and continuing with sample CRCs) and then read the next until a terminating number
		do
		{
			out.write((char *)&num, sizeof(unsigned long));

			// Get file name of wave file
			do
			{
				in.getline( wav, 32, '\n' );
			}
			while( !wav[0] && !(in.eof()) );

			i=0;
			while( wav[i] != '.' && wav[i] != '\0' && wav[i] != '\n' ) i++;
			wav[i] = '\0';

			// Output CRC of file if one specified, else output 0
			num = (strcmp(wav,"0")) ? UpdateCRC(wav) : 0;

			std::cerr << '.';

		} while( strcmp(wav,"end") ); // End of actor sample list

		std::cerr << ']' << std::endl;

		// Skip any blank lines before the next actor
		do
		{
			in.getline( actor, 32, '\n' );
		}
		while( !actor[0] && !(in.eof()) );
	}

	in.close();
	out.close();

	return 0;
}
