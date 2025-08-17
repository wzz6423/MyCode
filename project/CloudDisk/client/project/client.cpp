#include "client.h"

#define BACKUP_FILE "./backup.dat"
#define BACKUP_DIR "./backup/"
auto main() -> int
{
	CloudBackUp::BackUp::BackUp client(BACKUP_DIR, BACKUP_FILE);
	client.Run();

	return 0;
}