#ifndef VFS_H
#define VFS_H

#define VFS_MAX_FILE_NAME 11

struct node_struct {
	int size;
	char name[VFS_MAX_FILE_NAME+1];	// 11 + NULL TERMINATOR
};

struct vfs_header {
	unsigned int n_files;
	struct node_struct node[0]; 
};

#endif
