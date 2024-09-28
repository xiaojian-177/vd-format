#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义用于存储文件信息的结构体
typedef struct {
    size_t filename_len;
    size_t data_len;
} FileHeader;

// 将数据写入虚拟硬盘文件
int writevd(char* virtual_file, char* vd_file, char* data) {
    FILE *vd = fopen(vd_file, "ab+");
    if (vd == NULL) {
        perror("无法打开虚拟硬盘文件");
        return -1;
    }

    // 创建文件头并写入
    FileHeader header;
    header.filename_len = strlen(virtual_file);
    header.data_len = strlen(data);
    fwrite(&header, sizeof(FileHeader), 1, vd);

    // 写入文件名和数据
    fwrite(virtual_file, sizeof(char), header.filename_len, vd);
    fwrite(data, sizeof(char), header.data_len, vd);

    fclose(vd);
    return 0;
}

// 从虚拟硬盘文件中读取数据
char* readvd(char* virtual_file, char* vd_file) {
    FILE *vd = fopen(vd_file, "rb");
    if (vd == NULL) {
        perror("无法打开虚拟硬盘文件");
        return NULL;
    }

    FileHeader header;
    char *buffer = NULL;
    char *filename = NULL;

    // 遍历虚拟硬盘中的所有文件
    while (fread(&header, sizeof(FileHeader), 1, vd) == 1) {
        filename = (char*)malloc(header.filename_len + 1);
        if (filename == NULL) {
            perror("内存分配失败");
            fclose(vd);
            return NULL;
        }

        fread(filename, sizeof(char), header.filename_len, vd);
        filename[header.filename_len] = '\0';

        // 检查是否找到了要读取的文件
        if (strcmp(filename, virtual_file) == 0) {
            buffer = (char*)malloc(header.data_len + 1);
            if (buffer == NULL) {
                perror("内存分配失败");
                free(filename);
                fclose(vd);
                return NULL;
            }

            fread(buffer, sizeof(char), header.data_len, vd);
            buffer[header.data_len] = '\0';
            break;
        }

        free(filename);
        fseek(vd, header.data_len, SEEK_CUR); // 移动到下一个文件的开始位置
    }

    fclose(vd);
    if (filename != NULL) {
        free(filename);
    }

    return buffer;
}

