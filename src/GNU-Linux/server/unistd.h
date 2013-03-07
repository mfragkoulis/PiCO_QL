#ifndef _UNISTD_H
#define _UNISTD_H

#include <linux/unistd.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <stdlib.h>
#include <linux/stat.h>

#define NFILES 10
#define F_OK 0
#define R_OK 4
#define W_OK 2
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* Dummy macro and function 
 * implementations all over the file.
 * All action happens in-memory.
 */

#define geteuid() 0

#define fchown(X,Y,Z) 0

#define sleep(X) ssleep(X)

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

/* Close an open file descriptor */
//#define close(X) (((X = 0) == 0) ? X : -1)
int close(int fd);

/* Determine information about a file based on its 
 * file descriptor
 */
//#define fstat(X,Y) 0
int fstat(int fd, struct stat *buf);

/* Open new file and obtain its file descriptor */
#define open(X,Y, ...) 0

/* Remove file from file system */
//#define unlink(X) (((X = NULL) == NULL) ? 0 : -1)
int unlink(int fd);
/* Change the location of a read/write pointer 
 * for a file descriptor
 */
#define lseek(X,Y,Z) 0

/* Read data into a buffer */
//#define read(X,Y,Z) 0
int read(int fd, void *buf, size_t size);

/* Determine information about a file based 
 * on its file path. 
 */
//#define stat(X,Y) 0
int stat(const char *path, struct stat *buf);

/* Write data out of a buffer */
//#define write(X,Y,Z) 0
int write(int fd, const void *buf, size_t size);

/* Ensure I/O operations on file descriptor 
 * have finished. Compiled with SQLITE_NO_SYNC 
 * so this should be no op.
 */
#define fsync(X) 0
#define fdatasync(X) 0

/* Truncates the file indicated by the open 
 * file descriptor file_descriptor to the 
 * indicated length.
 */
//#define ftruncate(X,Y) 0
int ftruncate(int fd, off_t length);
#endif
