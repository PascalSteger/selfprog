#ifndef __md5__
#define __md5__

unsigned char result(unsigned char* md);
unsigned long get_size_by_fd(int fd);
char* calc_md5(char* filename);

#endif
