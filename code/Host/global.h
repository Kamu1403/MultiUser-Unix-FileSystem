#ifndef GLOBAL_H
#define GLOBAL_H

#include "Buffer.h"
#include "BufferManager.h"
#include "DeviceDriver.h"
#include "File.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "INode.h"
#include "OpenFileManager.h"
#include "User.h"
#include "Utility.h"
#include "stdc++.h"

using namespace std;

extern DeviceDriver g_DeviceDriver;
extern BufferManager g_BufferManager;
extern OpenFileTable g_OpenFileTable;
extern SuperBlock g_SuperBlock;
extern FileSystem g_FileSystem;
extern INodeTable g_INodeTable;
extern FileManager g_FileManager;
extern User g_User;
extern stringstream isstream, osstream;

#endif