#include <unistd.h>

int rmdir(const char *path){return 0;};

/* Returns an absolute file name representing 
 * the current working directory, storing it 
 * in the character array buffer that you provide.
 */
//#define getcwd(X,Y) 1
char *getcwd(char *buf, size_t size){return NULL;};

/* Check access permissions of file or path name */
//#define access(X,Y) ((X = 0) == 0)
int access(const char *path, int mode){return 0;};

/* Close an open file descriptor */
//#define close(X) (((X = 0) == 0) ? X : -1)
int close(int fd){return 0;};

/* Determine information about a file based on its 
 * file descriptor
 */
//#define fstat(X,Y) 0
int fstat(int fd, struct stat *buf){return 0;};

/* Remove file from file system */
//#define unlink(X) (((X = NULL) == NULL) ? 0 : -1)
int unlink(int fd){return 0;};

/* Read data into a buffer */
//#define read(X,Y,Z) 0
int read(int fd, void *buf, size_t size){return 0;};

/* Determine information about a file based 
 * on its file path. 
 */
//#define stat(X,Y) 0
int stat(const char *path, struct stat *buf){return 0;};

/* Write data out of a buffer */
//#define write(X,Y,Z) 0
int write(int fd, const void *buf, size_t size){return 0;};

/* Truncates the file indicated by the open 
 * file descriptor file_descriptor to the 
 * indicated length.
 */
//#define ftruncate(X,Y) 0
int ftruncate(int fd, off_t length){return 0;};
