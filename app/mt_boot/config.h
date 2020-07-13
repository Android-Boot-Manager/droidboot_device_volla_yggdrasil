
#ifndef __REBOOT_BOOT_ENTRY_H
#define __REBOOT_BOOT_ENTRY_H
struct global_config {
	char *default_entry_title;
	struct boot_entry *default_entry;
	int timeout;
};


struct boot_entry {
	char *title;
	char *linux;
	char *initrd;
    char *dtb;
	char *options;
	bool error;
};

int get_entry_count(void);

int parse_boot_entries(struct boot_entry **entry_list);

int parse_global_config(struct global_config *global_config);

#endif
