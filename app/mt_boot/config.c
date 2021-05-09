// SPDX-License-Identifier: GPL-2.0+
// © 2019 Mis012
// © 2020 luka177

#include <stdio.h>
#include <stdlib.h>
#include <lib/fs.h>
#include <string.h>
#include <err.h>
#include "config.h"

#define ENTRIES_DIR "/boot/db/entries"
#define GLOBAL_CONFIG_FILE "/boot/db/db.conf"

int config_parse_option(char **_dest, const char *option, const char *buffer) {
	char *temp = strstr(buffer, option);
	if(!temp)
		return -1;
	
	temp += strlen(option);
	while (*temp == ' ')
		temp++;
	char *newline = strchr(temp, '\n');
	if(newline)
		*newline = '\0';
	char *dest = malloc(strlen(temp) + 1);
	if(!dest)
		return ERR_NO_MEMORY;
	strcpy(dest, temp);
	*_dest = dest;

	//restore the buffer
	*newline = '\n';

	return 0;
}

int parse_boot_entry_file(struct boot_entry *entry, struct dirent ent) {
	int ret;
	filehandle *entry_file_handle = NULL;
	unsigned char *buf;

	char *path = malloc(strlen(ent.name) + strlen(ENTRIES_DIR"/") + 1);
	strcpy(path, ENTRIES_DIR"/");
	strcat(path, ent.name);

	off_t entry_file_size = fs_get_file_size(path);
	buf = malloc(entry_file_size + 1);
	if(!buf)
		return ERR_NO_MEMORY;

	ret = fs_open_file(path, &entry_file_handle);
	printf("fs_open_file ret: %d\n", ret);
	if(ret < 0) {
		return ret;
	}

	ret = fs_read_file(entry_file_handle, buf, 0, entry_file_size);
	printf("fs_read_file ret: %d\n", ret);
	if(ret < 0) {
		free(buf);
		return ret;
	}

	ret = fs_close_file(entry_file_handle);
	printf("fs_close_file ret: %d\n", ret);
	if(ret) {
		free(buf);
		return ret;
	}

	buf[entry_file_size] = '\0';
	
	ret = config_parse_option(&entry->title, "title", (const char *)buf);
	if(ret < 0) {
		printf("SYNTAX ERROR: entry \"%s\" - no option 'title'\n", path);
		free(buf);
		return ret;
	}

	ret = config_parse_option(&entry->linux, "linux", (const char *)buf);
	if(ret < 0) {
		printf("SYNTAX ERROR: entry \"%s\" - no option 'linux'\n", path);
		free(buf);
		return ret;
	}

	ret = config_parse_option(&entry->initrd, "initrd", (const char *)buf);
	if(ret < 0) {
		printf("SYNTAX ERROR: entry \"%s\" - no option 'initrd'\n", path);
		free(buf);
		return ret;
	}

	ret = config_parse_option(&entry->dtb, "dtb", (const char *)buf);
	if(ret < 0) {
		printf("SYNTAX ERROR: entry \"%s\" - no option 'dtb'\n", path);
		free(buf);
		return ret;
	}

	ret = config_parse_option(&entry->options, "options", (const char *)buf);
	if(ret < 0) {
		printf("SYNTAX ERROR: entry \"%s\" - no option 'options'\n", path);
		free(buf);
		return ret;
	}

	free(buf);

	entry->error = false;

	return 0;
}

int entry_count;

int get_entry_count1(void) {
	return entry_count;
}

int parse_boot_entries(struct boot_entry **_entry_list) {
	int ret;

	struct boot_entry *entry_list;

	ret = fs_mount("/boot", "ext2", "cache"); //system
	printf("fs_mount ret: %d\n", ret);
	if(ret) {
		return ret;
	}

	ret = entry_count = dir_count_entries(ENTRIES_DIR);
	if (ret < 0) {
		entry_count = 0;
		return ret;
	}

	entry_list = malloc(entry_count * sizeof(struct boot_entry));
	if(!entry_list) {
		entry_count = 0;
		return ERR_NO_MEMORY;
	}

	struct dirhandle *dirhandle;
	struct dirent ent;

	ret = fs_open_dir(ENTRIES_DIR, &dirhandle);
	if(ret < 0) {
		printf("fs_open_dir ret: %d\n", ret);
		entry_count = 0;
		return ret;
	}

	int i = 0;
	while(fs_read_dir(dirhandle, &ent) >= 0) {
		struct boot_entry *entry = entry_list + i;
		ret = parse_boot_entry_file(entry, ent);
		if(ret < 0) {
			printf("error passing entry: %s\n", ent.name);
			entry->error = true;
			entry->title = "SYNTAX ERROR";
		}
		i++;
	}
	fs_close_dir(dirhandle);

	fs_unmount("/boot");
	
	*_entry_list = entry_list;
	
	return 0;
}

int parse_global_config(struct global_config *global_config) {
	int ret;
	filehandle *global_config_file_handle = NULL;
	unsigned char *buf;

	ret = fs_mount("/boot", "ext2", "cache"); //system
	printf("fs_mount ret: %d\n", ret);
	if(ret) {
		return ret;
	}

	off_t entry_file_size = fs_get_file_size(GLOBAL_CONFIG_FILE);
	buf = malloc(entry_file_size + 1);
	if(!buf)
		return ERR_NO_MEMORY;

	ret = fs_open_file(GLOBAL_CONFIG_FILE, &global_config_file_handle);
	printf("fs_open_file ret: %d\n", ret);
	if(ret < 0) {
		return ret;
	}

	ret = fs_read_file(global_config_file_handle, buf, 0, entry_file_size);
	printf("fs_read_file ret: %d\n", ret);
	if(ret < 0) {
		free(buf);
		return ret;
	}

	ret = fs_close_file(global_config_file_handle);
	printf("fs_close_file ret: %d\n", ret);
	if(ret) {
		free(buf);
		return ret;
	}

	ret = config_parse_option(&global_config->default_entry_title, "default", (const char *)buf);
	if(ret < 0) {
		printf("WARNING: lk2nd.conf: - no option 'default'\n");

		global_config->default_entry_title = NULL;
		global_config->timeout = 0;

		fs_unmount("/boot");
		return 0;
	}

	char *timeout = NULL;
	ret = config_parse_option(&timeout, "timeout", (const char *)buf);
	if(ret < 0) {
		printf("SYNTAX ERROR: lk2nd.conf - no option 'timeout'\n");
	}

	global_config->timeout = atoi(timeout);
	fs_unmount("/boot");

	return 0;
}
