// x86-16
// I SAWED THIS EMULATOR IN HALF
// © 2024 starfrost			January 7, 2024 

#include "core.h"
#include "cmd/cmd.h"
#include "x86/8086/8086.h"

int main(int argc, char* argv[])
{
	Logging_Init();

	Logging_LogAll("modern:personality Runtime v%s", X86_VERSION);
	Logging_LogAll("©2024 starfrost");

	if (!CMD_Parse(argc, argv))
	{
		//todo: print help
		exit(1);
	}
	i8086_Init();

	bool os_init = false;

	if (cmd.is_mtdos)
	{
		os_init = MTDOS_Init();
	}
	else
	{
		os_init = MSDOS_Init();
	}

	if (!os_init)
	{
		exit(2);
	}

	// init complete, run
	machine_running = true;
	
	while (machine_running)
	{
		i8086_Update();
	}

	return 0;
}
