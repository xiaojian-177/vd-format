#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// �������ڴ洢�ļ���Ϣ�Ľṹ��
typedef struct {
    size_t filename_len;
    size_t data_len;
} FileHeader;

// ������д������Ӳ���ļ�
int writevd(char* virtual_file, char* vd_file, char* data) {
    FILE *vd = fopen(vd_file, "ab+");
    if (vd == NULL) {
        perror("�޷�������Ӳ���ļ�");
        return -1;
    }

    // �����ļ�ͷ��д��
    FileHeader header;
    header.filename_len = strlen(virtual_file);
    header.data_len = strlen(data);
    fwrite(&header, sizeof(FileHeader), 1, vd);

    // д���ļ���������
    fwrite(virtual_file, sizeof(char), header.filename_len, vd);
    fwrite(data, sizeof(char), header.data_len, vd);

    fclose(vd);
    return 0;
}

// ������Ӳ���ļ��ж�ȡ����
char* readvd(char* virtual_file, char* vd_file) {
    FILE *vd = fopen(vd_file, "rb");
    if (vd == NULL) {
        perror("�޷�������Ӳ���ļ�");
        return NULL;
    }

    FileHeader header;
    char *buffer = NULL;
    char *filename = NULL;

    // ��������Ӳ���е������ļ�
    while (fread(&header, sizeof(FileHeader), 1, vd) == 1) {
        filename = (char*)malloc(header.filename_len + 1);
        if (filename == NULL) {
            perror("�ڴ����ʧ��");
            fclose(vd);
            return NULL;
        }

        fread(filename, sizeof(char), header.filename_len, vd);
        filename[header.filename_len] = '\0';

        // ����Ƿ��ҵ���Ҫ��ȡ���ļ�
        if (strcmp(filename, virtual_file) == 0) {
            buffer = (char*)malloc(header.data_len + 1);
            if (buffer == NULL) {
                perror("�ڴ����ʧ��");
                free(filename);
                fclose(vd);
                return NULL;
            }

            fread(buffer, sizeof(char), header.data_len, vd);
            buffer[header.data_len] = '\0';
            break;
        }

        free(filename);
        fseek(vd, header.data_len, SEEK_CUR); // �ƶ�����һ���ļ��Ŀ�ʼλ��
    }

    fclose(vd);
    if (filename != NULL) {
        free(filename);
    }

    return buffer;
}

