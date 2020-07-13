#include <stdlib.h>
#include <stdio.h>

#include <lib/fs.h>

off_t fs_get_file_size(const char *filename) {
	filehandle *file_handle = NULL;
	struct file_stat file_stat;

	int ret;

	ret = fs_open_file(filename, &file_handle);
	if(ret < 0)
		return 0;

	ret = fs_stat_file(file_handle, &file_stat);
	if(ret < 0)
		return 0;

	ret = fs_close_file(file_handle);
	if (ret < 0)
		return 0;

	return file_stat.size;
}

int dir_count_entries(const char *path) {
	struct dirhandle *dirhandle;
	struct dirent ent;

	int entry_count = 0;

	int ret;

	ret = fs_open_dir(path, &dirhandle);
	if(ret < 0) {
		printf("fs_open_dir ret: %d (path: %s)\n", ret, path);
		return ret;
	}

	while(fs_read_dir(dirhandle, &ent) >= 0) {
		entry_count++;
	}
	fs_close_dir(dirhandle);

	return entry_count;
}
