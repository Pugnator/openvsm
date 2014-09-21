#include <tui.h>

void debug_console_alloc ( void )
{

	if ( AllocConsole() == FALSE )
		return;
		
	HANDLE hnd = GetStdHandle ( STD_INPUT_HANDLE );
	if ( !FlushConsoleInputBuffer ( hnd ) )
	{
		hnd = CreateFile ( "CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL );
		SetStdHandle ( STD_INPUT_HANDLE,hnd );
		hnd = CreateFile ( "CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL );
		SetStdHandle ( STD_OUTPUT_HANDLE,hnd );
		hnd = CreateFile ( "CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL );
		SetStdHandle ( STD_ERROR_HANDLE,hnd );
	}
	//SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
	
	initscr();
	//HANDLE hConsole = GetConsoleWindowHandle();
	//SetConsoleScreenBufferSize(hConsole,coord);
	//wresize(con, 30, 30);
	//No buffer for getch()
	cbreak( );
	raw( );
	nonl( );
	//no echo while entering from keyboard
	noecho( );
	//no text cursor
	curs_set ( 0 );
	//Enable special keys
	keypad ( stdscr, TRUE );
	idlok ( stdscr,TRUE );
	scrollok ( stdscr,TRUE );
	start_color();
	//set main colors
	init_pair ( 1, COLOR_WHITE, COLOR_BLUE );
	init_pair ( 2, COLOR_CYAN, COLOR_CYAN );
	init_pair ( 3, COLOR_YELLOW, COLOR_WHITE );
	init_pair ( 4, COLOR_RED, COLOR_WHITE );
	init_pair ( 5, COLOR_GREEN, COLOR_WHITE );
	init_pair ( 6, COLOR_BLUE, COLOR_WHITE );
	init_pair ( 7, COLOR_WHITE, COLOR_BLACK );
	init_pair ( 8, COLOR_MAGENTA, COLOR_WHITE );
	init_pair ( 9, COLOR_CYAN, COLOR_WHITE );
	init_pair ( 10, COLOR_YELLOW, COLOR_BLACK );
	//set default colors
	assume_default_colors ( COLOR_BLACK, COLOR_WHITE );
	wbkgd ( stdscr, COLOR_PAIR ( 5 ) );
	console_log ( "%s\n", "Debugger started" );
	refresh();
	
}

void debug_console_free ( void )
{
	endwin();
	FreeConsole();
	
}

void console_log ( char* format, ... )
{
	char* string;
	va_list args;
	
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, format, args ) ) string = NULL;
	va_end ( args );
	printw ( "%s", string );
	free ( string );
	refresh();
}
