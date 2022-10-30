#include "DeviceDriver.h"

const char * DeviceDriver::DISK_FILE_NAME = "myDisk.img";

DeviceDriver::DeviceDriver() {
    fp = fopen(DISK_FILE_NAME, "rb+");
}

DeviceDriver::~DeviceDriver() {
    if (fp) {
        fclose(fp);
    }
}

/* ��龵���ļ��Ƿ���� */
bool DeviceDriver::Exists() {
    return fp != NULL;
}

/* �򿪾����ļ� */
void DeviceDriver::Construct() {
    if (fp == NULL) {
        fp = fopen(DISK_FILE_NAME, "wb+");
        printf("���������ļ���%s", DISK_FILE_NAME);
    }
}

/* ʵ��д���̺��� */
void DeviceDriver::write(const void* buffer, unsigned int size, int offset, unsigned int origin) {
    if (offset >= 0) {
        fseek(fp, offset, origin);
    }
    fwrite(buffer, size, 1, fp);
}

/* ʵ��д���̺��� */
void DeviceDriver::read(void* buffer, unsigned int size, int offset, unsigned int origin) {
    if (offset >= 0) {
        fseek(fp, offset, origin);
    }
    fread(buffer, size, 1, fp);
}