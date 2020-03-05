#include "fs_determ.h"

void get_bd_fs_type(char* path, char* type)
{
    blkid_probe pr;
    pr = blkid_new_probe_from_filename(path);
    if(!pr) {
        printf("Err: Failed to open device\n");
        return;
    }

    const char* _type = NULL;
    blkid_do_probe(pr);
    blkid_probe_lookup_value(pr, "TYPE", &_type, NULL);

    if(_type)
        strcpy(type, _type);

    blkid_free_probe(pr);
}

/*
int main()
{
    char* path = "/dev/sdc1";
    char type[50] = ""; 

    get_bd_fs_type(path, type);

    printf("FILE: %s has type %s\n", path, type);
}
*/
