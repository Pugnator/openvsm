#include <windows.h>
#include <stdio.h>
#include "SFML/Audio.h"

int main()
{
	sfMusic *buf = sfMusic_createFromFile("c:\\new.ogg");
	if (NULL == buf)
	{
		puts("Failed");
	}
	
	sfMusic_play(buf);
	getchar();
	sfMusic_destroy(buf);
	return 0;
}
