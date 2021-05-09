#ifndef __REBOOT_FS_UTIL_H
#define __REBOOT_FS_UTIL_H

off_t fs_get_file_size(const char *filename);
int dir_count_entries(char *path);

#endif
