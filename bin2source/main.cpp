#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <Shlwapi.h>
#define DELIM '\\'
#elif
#define DELIM '/'
#endif

/** \brief	The filename[ maximum path]. */
char filename[MAX_PATH] = {0};

/**********************************************************************************************//**
 * \fn	char *basename(char *path)
 *
 * \brief	Basenames the given file.
 *
 * \author	Pugnator
 * \date	11/21/2015
 *
 * \param [in,out]	path	If non-null, full pathname of the file.
 *
 * \return	null if it fails, else a char*.
 **************************************************************************************************/

char 
*basename(char *path)
{
	char *p;
	if (path == NULL)
		return path;

	for (p = path + strlen(path); --p > path && *p == DELIM;)
	{
		*p = '\0';
	}
	p = strrchr(path, DELIM);
	if (DELIM == *p)
	{
		p++;
	}
	return p ? p : path;
}

/**********************************************************************************************//**
 * \fn	void make_ident(char* name)
 *
 * \brief	Makes an identifier.
 *
 * \author	Pugnator
 * \date	11/21/2015
 *
 * \param [in,out]	name	If non-null, the name.
 **************************************************************************************************/

void
make_ident(char* name)
{	
	strncpy(filename, basename(name), sizeof filename);	
	for (char *p = filename; *p; p++)
	{
		if (!isalnum(*p)) 
		{
			*p = '_';
		}
	}	
}

int
main(int argc, char* argv[])
{	
#if 0
	for (;;)
	{
		if (IsDebuggerPresent())
		{
			__debugbreak();
			break;
		}
		Sleep(1000);
	}
#endif // 0

	unsigned char buf[BUFSIZ];

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s binary_file > output_file\n", argv[0]);
		return -1;
	}

	FILE *fd = fopen(argv[1], "rb");
	if (!fd)
	{
		fprintf(stderr, "%s: can't open %s for reading\n", argv[0], argv[1]);
		return -1;
	}
	
	make_ident(argv[1]);
	size_t rd = 0;
	size_t totalsize = 0;
	printf("const unsigned char %s[] = {", filename);
	for (int total = 0, need_comma = 0; (rd = fread(buf, 1, BUFSIZ, fd)) > 0; )
	{
		totalsize += rd;
		if (!rd)
		{
			fprintf(stderr, "%s: file read error\n", argv[0]);
			return -1;
		}
		for (size_t i = 0; i < rd; i++)
		{
			if (need_comma) printf(", ");
			else need_comma = 1;
			if ((total % 11) == 0) printf("\n\t");
			printf("0x%.2x", buf[i]);
			total++;
		}
	}
	printf("\n};\n");
	printf("const unsigned %s_len = %zu;\n", filename, totalsize);	
	return 0;
}
