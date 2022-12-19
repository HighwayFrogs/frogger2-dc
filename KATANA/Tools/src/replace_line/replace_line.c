/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t getfilelength(FILE* fp);
int replaceinfile(char* originalfile, char* destinationfile, char* placeholder, char* replacement);
char* unescape(char* input);

int main(int argc, char* argv[])
{
	if (argc <= 4)
	{
		printf("Usage: %s <in_file> <out_file> <template_str> <replace_str>\n", argv[0]);
		printf("Example: %s input.txt output.txt \"%%PLACEHOLDER%%\" \"replacement string\"\n", argv[0]);
		exit(1);
	}

	char* inputFileName = argv[1];
	char* outputFileName = argv[2];
	char* templateStr = unescape(argv[3]);
	char* replacementStr = unescape(argv[4]);

	int ret = replaceinfile(inputFileName, outputFileName, templateStr, replacementStr);
	free(templateStr);
	free(replacementStr);

	if (ret == 0)
	{
		puts("Could not open file.");
		return 1;
	}

	return 0;
}


size_t getfilelength(FILE* fp)
{
	/*
	 * return the number of bytes in file fp
		 * value of 0 returned is empty or error
	 * it doesn't matter for our purpose
	 */
	size_t retval;
	if (!fp)
		retval = 0;
	else
	{
		fseek(fp, 0, SEEK_END);
		retval = ftell(fp);
		fseek(fp, 0, SEEK_SET);
	}
	return retval;
}

int replaceinfile(char* originalfile, char* destinationfile, char* placeholder, char* replacement)
{
	/*
	 * write originalfile into destinationfile replacing every instance
	 * of placeholder with replacement
	 */

	 /* open files */
	FILE* input = fopen(originalfile, "r");
	if (!input)
		return 0; // Failed to open input file.

	int ch;
	char* substr;
	size_t filesize = getfilelength(input);
	char* filebuffer = (char*)malloc(filesize + 1);
	if (!filebuffer)
		return 0;

	size_t read_bytes = fread(filebuffer, 1, filesize, input);
	fclose(input); // Done with input file.

	// Write new file.
	FILE* destination = fopen(destinationfile, "w");
	substr = strstr(filebuffer, placeholder);
	for (size_t i = 0; i < read_bytes; i++)
	{
		if (substr && i == (substr - filebuffer))
		{
			i += strlen(placeholder) - 1;
			substr++;
			substr = strstr(substr, placeholder);
			fprintf(destination, "%s", replacement);
		}
		else
		{
			ch = filebuffer[i];
			fputc(ch, destination);
		}
	}

	fclose(destination);
	free(filebuffer);
	return 1;
}

char* unescape(char* input)
{
	size_t len = strlen(input);
	char* result = (char*)malloc(len + 1);
	char* outPos = result;
	char* inPos = input;
	char* endPos = input + len;

	char current;
	while (inPos < endPos) {
		current = *(inPos++);

		if (current == '\\' && inPos < endPos) {
			current = *(inPos++);
			if (current == 't') {
				*(outPos++) = '\t';
			} else if (current == 'r') {
				*(outPos++) = '\r';
			} else if (current == 'n') {
				*(outPos++) = '\n';
			} else if (current == '\\') {
				*(outPos++) = '\\';
			} else {
				*(outPos++) = '\\';
				*(outPos++) = current;
			}
		} else {
			*(outPos++) = current;
		}
	}
	*outPos = '\0';

	return result;
}