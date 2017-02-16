#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv)
{
    int fd, changes, i, random_spot, kids[2];
    struct stat buf;
    char *the_file,
         * starting_string = "Using mmap() can be fun\nand informative!";

    if (argc !=3 )
    {
        fprintf(stderr, "Usage %s file_name #_of_changes\n", *argv);
        exit(1);
    }
    if (( changes = atoi(argv[2])) < 1){
        fprintf(stderr, "# of changes < 1\n");
    }
    if (( fd = open(argv[1], O_CREAT|O_RDWR, 0666)) < 0) {
        fprintf(stderr, "open error on file %s\n", *argv);
        exit(3);
    }
    write(fd, starting_string, strlen(starting_string));
    if (fstat(fd, &buf) < 0)
    {
        fprintf(stderr, "fstat error on file %s\n", *argv);
        exit(4);
    }
    if (( the_file = mmap(0, (size_t) buf.st_size, PROT_READ|PROT_WRITE,
            MAP_SHARED, fd, 0) ) == (caddr_t) -1){
        fprintf(stderr, "mmap failure\n");
        exit(5);
    }
    for (i = 0; i < 2; i++){
        if (( kids[i] = (int) fork() ) == 0)
            while(1) {
                printf("Child %d finds:\n%s\n", getpid(), the_file);
                sleep(1);
            }
    }
    srand((unsigned ) getpid());
    for (i = 0; i< changes; ++i){
        random_spot = (int) (rand() % buf.st_size);
        *(the_file + random_spot) = '*';
        sleep(1);
    }
    printf("Parent done with changes\n");
    for (i=0; i < 2; i++)
        kill (kids[i], 9);
    printf("The file now contains:\n%s\n", the_file);
    exit(0);
}
