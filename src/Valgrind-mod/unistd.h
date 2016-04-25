#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdlib.h>
#include "pub_tool_vki.h"  // uuid_t, vki_mode_t, VKI_PAGE_SIZE
#include "pub_tool_libcfile.h"  // mknod, open, close, read, write, unlink

#define NFILES 10
#define F_OK 0
//#define R_OK 4
//#define W_OK 2
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


#ifdef __linux__
#define PAGE_SIZE VKI_PAGE_SIZE 
#include <sys/stat.h>
#include "pub_tool_basics.h" // OffT (instead of off_t)
// Left out for compiling on Stereo (mfg - 30/05/2015)
//typedef vki_mode_t mode_t;  /* mode_t not defined */
//typedef OffT off_t;
struct statfs;

/* VG: matched: Open new file and obtain its file descriptor */
int open(const char *path, int flags, ...);
int open64(const char *path, int flags, ...);
int fcntl(int n, int m, ...);
int fstatfs(int fd, struct statfs *buf);
int statfs(const char *fs, struct statfs *buf);
#endif

/* Dummy macro and function 
 * implementations all over the file.
 * All action happens in-memory.
 */

#define geteuid() 0

#define fchown(X,Y,Z) 0

int fchmod(int fildes, mode_t mode);
//#define fchmod(X,Y) 0

#define sleep(X) (void)X
#define usleep(X) (void)X

int rmdir(const char *path);

/* Returns an absolute file name representing 
 * the current working directory, storing it 
 * in the character array buffer that you provide.
 */
//#define getcwd(X,Y) 1
char *getcwd(char *buf, size_t size);

/* Check access permissions of file or path name */
//#define access(X,Y) ((X = 0) == 0)
int access(const char *path, int mode);

/* VG: matched: Create pipe with mknod */
#define mknod(X,Y,Z) VG_(mknod)(X,Y,Z)

/* VG: matched: Close an open file descriptor */
//#define close(X) VG_(close)(X)
void close(int fd);

/* Determine information about a file based on its 
 * file descriptor
 */
//#define fstat(X,Y) 0
int fstat(int fd, struct stat *buf);


/* VG: match: Remove file from file system */
//#define unlink(X) VG_(unlink)(X)
int unlink(const char *name);
/* Change the location of a read/write pointer 
 * for a file descriptor
 */
#define lseek(X,Y,Z) 0

/* VG: match: Read data into a buffer */
//#define read(X,Y,Z) VG_(read)(X,Y,Z)
int read(int fd, void *buf, size_t size);
int pread(int fd, void *buf, size_t size, off_t offset);

/* Determine information about a file based 
 * on its file path. 
 */
//#define stat(X,Y) 0
int stat(const char *path, struct stat *buf);

/* VG: matched: Write data out of a buffer */
//#define write(X,Y,Z) VG_(write)(X,Y,Z)
int write(int fd, const void *buf, size_t size);
int pwrite(int fd, const void *buf, size_t size, off_t offset);

/* Ensure I/O operations on file descriptor 
 * have finished. Compiled with SQLITE_NO_SYNC 
 * so this should be no op.
 */
int fsync(int fd);
#define fdatasync(X) 0

/* Truncates the file indicated by the open 
 * file descriptor file_descriptor to the 
 * indicated length.
 */
//#define ftruncate(X,Y) 0
int ftruncate(int fd, off_t length);

// for __APPLE__
int fsctl(const char *path, unsigned long request, void *data,
	     unsigned long options);

#endif
