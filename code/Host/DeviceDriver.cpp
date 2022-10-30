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

/* 检查镜像文件是否存在 */
bool DeviceDriver::Exists() {
    return fp != NULL;
}

/* 打开镜像文件 */
void DeviceDriver::Construct() {
    if (fp == NULL) {
        fp = fopen(DISK_FILE_NAME, "wb+");
        printf("创建磁盘文件：%s", DISK_FILE_NAME);
    }
}

/* 实际写磁盘函数 */
void DeviceDriver::write(const void* buffer, unsigned int size, int offset, unsigned int origin) {
    if (offset >= 0) {
        fseek(fp, offset, origin);
    }
    fwrite(buffer, size, 1, fp);
}

/* 实际写磁盘函数 */
void DeviceDriver::read(void* buffer, unsigned int size, int offset, unsigned int origin) {
    if (offset >= 0) {
        fseek(fp, offset, origin);
    }
    fread(buffer, size, 1, fp);
}