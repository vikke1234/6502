#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H
struct file {
    int8_t header[16];
    int8_t trainer[512];
    int8_t PRGrom[16384];
    int8_t CHRrom[8192];
    int8_t INSTrom[8192];
};

struct file *read_rom(char *path);
void free_rom(struct file *f);
#endif // FILE_HANDLER_H
