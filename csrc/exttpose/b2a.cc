#include <iostream>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv)
{
   int fd;
   if ((fd = open(argv[1], O_RDONLY)) < 0){
      throw runtime_error("cant openfile ");
   }
   long flen = lseek(fd, 0, SEEK_END);
   int *ary;
#ifdef SGI
   ary = (int *) mmap((char *)NULL, flen,
                          (PROT_WRITE|PROT_READ),
                          MAP_PRIVATE, fd, 0);
#else
   ary = (int *) mmap((char *)NULL, flen,
                          (PROT_WRITE|PROT_READ),
                          (MAP_FILE|MAP_VARIABLE|MAP_PRIVATE), fd, 0);
#endif
   if (ary == (int *)-1){
      throw runtime_error("MMAP ERROR");
   }
   for (int i=0; i < flen/sizeof(int); i++)
      cout << " " << ary[i];
   cout << endl;

   munmap((caddr_t)ary, flen);
   close(fd);
   
}
