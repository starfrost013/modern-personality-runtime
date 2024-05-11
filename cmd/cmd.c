#include "cmd.h"
#include "macros.h"
#include "dos/dos.h"
#include "util/logging.h"
#include "util/util.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

cmd_t cmd;

bool CMD_Parse(int argc, char* argv[])
{
	memset(&cmd, 0x00, sizeof(cmd_t));

	Logging_LogChannel("Beginning to parse command line...", LogChannel_Debug);

	for (int argn = 0; argn < argc; argn++)
	{
		// arguments that have another part after them
		if (argc - argn >= 1)
		{
			if (!strncmp(argv[argn], "-dosver", 7))
			{
				// THIS IS SAFE CODE I SWEAR
				switch (atoi(argv[argn + 1]))
				{
				case DOS_1x:
					Logging_LogAll("Simulated environment: MS-DOS 1.x");
					cmd.msdos_ver = DOS_1x;
					break;

				case DOS_20:
					Logging_LogAll("Simulated environment: MS-DOS 2.0 or 2.1");
					cmd.msdos_ver = DOS_20;
					break;

				case DOS_211:
					Logging_LogAll("Simulated environment: MS-DOS 2.11");
					cmd.msdos_ver = DOS_211;
					break;

				case DOS_225:
					Logging_LogAll("Simulated environment: MS-DOS 2.25");
					cmd.msdos_ver = DOS_225;
					break;

				case DOS_30:
					Logging_LogAll("Simulated environment: MS-DOS 3.0");
					cmd.msdos_ver = DOS_30;
					break;

				case DOS_31:
					Logging_LogAll("Simulated environment: MS-DOS 3.1");
					cmd.msdos_ver = DOS_31;
					break;

				case DOS_32:
					Logging_LogAll("Simulated environment: MS-DOS 3.2");
					cmd.msdos_ver = DOS_32;
					break;

				case DOS_33:
					Logging_LogAll("Simulated environment: MS-DOS 3.3x");
					cmd.msdos_ver = DOS_33;
					break;
				
				case DOS_40:
					Logging_LogAll("Simulated environment: MS-DOS 4.0");
					cmd.msdos_ver = DOS_30;
					break;

				case DOS_50:
					Logging_LogAll("Simulated environment: MS-DOS 5.0");
					cmd.msdos_ver = DOS_30;
					break;

				case DOS_60:
					Logging_LogAll("Simulated environment: MS-DOS 6.0-6.2");
					cmd.msdos_ver = DOS_30;
					break;

				case DOS_621:
					Logging_LogAll("Simulated environment: MS-DOS 6.21");
					cmd.msdos_ver = DOS_30;
					break;

				case DOS_622:
					Logging_LogAll("Simulated environment: MS-DOS 6.22");
					cmd.msdos_ver = DOS_30;
					break;
				}

				argn++; // skip the provided api level
				continue; 
			}
			else if (!strncmp(argv[argn], "-mtdosver", 9))
			{
				// unsafe code dont do this
				switch (atoi(argv[argn + 1]))
				{
				case MTDOS_Beta_5_29_84:
					Logging_LogAll("Simulated Environment: M/T-MSDOS Beta Release (May 29, 1984)");
					cmd.msdos_ver = MTDOS_Beta_5_29_84;
					break;
				case MTDOS_Hypothetical_Prerelease:
					Logging_LogAll("Simulated Environment: MT-DOS (Hypothetical pre-release with named pipes and multithreading; ~Early 1985)");
					cmd.msdos_ver = MTDOS_Hypothetical_Prerelease;
					break;
				case MTDOS_Beta_6_7:
					Logging_LogAll("Simulated Environment: MT-DOS 4.0 (Pre-release build 6.7, November 26, 1985)");
					cmd.msdos_ver = MTDOS_Beta_6_7;
					break;
				case MTDOS_Final:
					Logging_LogAll("Simulated Environment: MT-DOS 4.0 (November 17, 1986)");
					cmd.msdos_ver = MTDOS_Final;
					break;
				case MTDOS_Final_4_1:
					Logging_LogAll("Simulated Environment: MT-DOS 4.10-4.10.30 (October 11, 1987-May 10, 1988)");
					cmd.msdos_ver = MTDOS_Final_4_1;
					break;

				}

				argn++;
				continue;
			}

		}
		
		// arguments that don't have another part after them
		if (!strncmp(argv[argn], "-mtdos", 6))
		{
			cmd.is_mtdos = true;
			break;
		}
		else if (argn > 1) // program to run
		{
			// every arg after program to run is interpreted as arguments for the program
			// this is in the DOS PSP format
			char command_line_tmp[MAX_PATH_MODERN];
			memset(command_line_tmp, 0x00, sizeof(char)* MAX_PATH_MODERN);

			// immediately end processing

			for (int argn_file = argn; argn_file < argc; argn_file++)
			{
				// ignore first arg (which is the name of the process itself)
				if (argn_file < 1) argn_file = 1;

				// add a space
				strncat(command_line_tmp, " ", MAX_PATH_MODERN - strlen(command_line_tmp));
				strncat(command_line_tmp, argv[argn_file], MAX_PATH_MODERN - strlen(command_line_tmp));
			}

			// trim spaces
			char* command_line_tmp_trim = Util_StringLTrim(command_line_tmp);

			// copy it to cmd_t structure
			strncpy(cmd.command_line, command_line_tmp_trim, MAX_PATH_MODERN);

			// ensured in the case of no parsing by the memset line above
			if (strlen(cmd.command_line) == 0) return false;

			// open handle for read
			cmd.handle = fopen(cmd.command_line, "rb");

			if (cmd.handle == NULL)
			{
				// if the file wasn't found log a special error message
				if (errno == ENOENT)
				{
					Logging_LogChannel("File %s not found!", LogChannel_Fatal, cmd.command_line);
				}
				else
				{
					Logging_LogChannel("Failed to open %s: errno %d", LogChannel_Fatal, cmd.command_line, errno);
				}

				return false;
			}

			Logging_LogAll("Loading binary %s", cmd.command_line);
			return true; 
		}
	}

	return false; 
}