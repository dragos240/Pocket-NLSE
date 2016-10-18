#ifndef NLSE_BACKUP_H
#define NLSE_BACKUP_H

extern FS_Archive game_arch;
extern FS_Archive sdmc_arch;

Result backup_init();
Result backup_fini();

char* game_to_buffer(); //game -> buffer
void buffer_to_game(char* buffer); //buffer -> game

void buffer_to_dir(char* buffer, char* dirname); //buffer -> dir
char* dir_to_buffer(char* dirname); //dir -> buffer

#endif
