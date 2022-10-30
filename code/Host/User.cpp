#include "global.h"
using namespace std;

User::User() {
    u_error = U_NOERROR;
    fileManager = &g_FileManager;
	dirp = "/";
    curDirPath = "/";
    //fileManager->Open();
	cdir = fileManager->rootDirINode;
    pdir = NULL;
    i_uid = 1;
    username = "unix";
    Utility::memset(arg, 0, sizeof(arg));
}

User::~User() {
}


void User::Mkdir(string dirName) {
    if (!checkPathName(dirName)) {
        return;
    }
    arg[1] = INode::IFDIR;
    fileManager->Creat();
    GetLastError();
}

void User::Ls(bool verbose) {
    ls.clear();
    arg[1] = verbose;
    fileManager->Ls();
    if (GetLastError()) {
        return;
    }
    osstream << ls << endl;
}

void User::Cd(string dirName) {
    if (!checkPathName(dirName)) {
        return;
    }
    fileManager->ChDir();
    GetLastError();
}

void User::Create(string fileName, string mode) {
    if (!checkPathName(fileName)) {
        return;
    }
    int md = INodeMode(mode);
    if (md == 0) {
        osstream << "this mode is undefined !  \n";
        return;
    }

    arg[1] = md;
    fileManager->Creat();
    GetLastError();
}

void User::Delete(string fileName) {
    if (!checkPathName(fileName)) {
        return;
    }
    fileManager->UnLink();
    GetLastError();
}

void User::Open(string fileName, string mode) {
    if (!checkPathName(fileName)) {
        return;
    }
    int md = FileMode(mode);
    if (md == 0) {
        osstream << "this mode is undefined ! \n";
        return;
    }

    arg[1] = md;
    fileManager->Open();
    if (GetLastError())
        return;
    osstream << "open success, return fd=" << ar0[EAX] << endl;
}

void User::Close(string sfd) {
    if (sfd.empty() || !isdigit(sfd.front())) {
        osstream << "parameter fd can't be empty or be nonnumeric ! \n";
        return;
    }
    arg[0] = stoi(sfd);;
    fileManager->Close();
    GetLastError();
}

void User::Seek(string sfd, string offset, string origin) {
    if (sfd.empty() || !isdigit(sfd.front())) {
        osstream << "parameter fd can't be empty or be nonnumeric ! \n";
        return;
    }
    if (offset.empty()) {
        osstream << "parameter offset can't be empty ! \n";
        return;
    }
    if (origin.empty() || !isdigit(origin.front())) {
        osstream << "parameter origin can't be empty or be nonnumeric ! \n";
        return;
    }
    arg[0] = stoi(sfd);
    arg[1] = stoi(offset);
    arg[2] = stoi(origin);
    fileManager->Seek();
    GetLastError();
}

void User::Write(string sfd, string inFile, string size) {
    if (sfd.empty() || !isdigit(sfd.front())) {
        osstream << "parameter fd can't be empty or be nonnumeric ! \n";
        return;
    }
    int fd = stoi(sfd);
    char* buffer;
    int usize;

    if (inFile == "") {
        usize = size.length();
        buffer = new char[usize+1];
        strcpy(buffer, size.c_str());
    }
    else
    {
        if (size.empty() || (usize = stoi(size)) < 0) {
            osstream << "parameter size must be greater or equal than 0 ! \n";
            return;
        }

        buffer = new char[usize];
        fstream fin(inFile, ios::in | ios::binary);
        if (!fin) {
            osstream << "file " << inFile << " open failed ! \n";
            return;
        }
        fin.read(buffer, usize);
        fin.close();
    }



    arg[0] = fd;
    arg[1] = (long)buffer;
    arg[2] = usize;
    fileManager->Write();

    if (GetLastError())
        return;
    osstream << "write " << ar0[User::EAX] << "bytes success !" << endl;
    delete[]buffer;
}

void User::Read(string sfd, string outFile, string size) {
    if (sfd.empty() || !isdigit(sfd.front())) {
        osstream << "parameter fd can't be empty or be nonnumeric ! \n";
        return;
    }
    int fd = stoi(sfd);

    int usize;
    if (size.empty() || !isdigit(size.front()) || (usize = stoi(size)) < 0) {
        osstream << "parameter size is not right \n";
        return;
    }
    char *buffer = new char[usize];
    //osstream << "fd = " << fd << " outFile = " << outFile << " size = " << size << "\n";
    arg[0] = fd;
    arg[1] = (long)buffer;
    arg[2] = usize;
    fileManager->Read();
    if (GetLastError())
        return;

    osstream << "read " << ar0[User::EAX] << " bytes success : \n" ;
    if (outFile.empty()) {
        for (unsigned int i = 0; i < ar0[User::EAX]; ++i) {
            osstream << (char)buffer[i];
        }
        osstream << " \n";
        return;
    }
    fstream fout(outFile, ios::out | ios::binary);
    if (!fout) {
        osstream << "file " << outFile << " open failed ! \n";
        return;
    }
    fout.write(buffer, ar0[User::EAX]);
    fout.close();
    osstream << "read to " << outFile << " done ! \n";
    delete[]buffer;
}

int User::INodeMode(string mode) {
    int md = 0;
    if (mode.find("-r") != string::npos) {
        md |= (INode::IREAD | (INode::IREAD>>6));
    }
    if (mode.find("-w") != string::npos) {
        md |= (INode::IWRITE | (INode::IWRITE >> 6));
    }
    if (mode.find("-rw") != string::npos) {
        md |= (INode::IREAD | INode::IWRITE | (INode::IREAD >> 6) | (INode::IWRITE >> 6));
    }
    return md;
}

int User::FileMode(string mode) {
    int md = 0;
    if (mode.find("-r") != string::npos) {
        md |= File::FREAD;
    }
    if (mode.find("-w") != string::npos) {
        md |= File::FWRITE;
    }
    if (mode.find("-rw") != string::npos) {
        md |= (File::FREAD | File::FWRITE);
    }
    return md;
}

bool User::checkPathName(string path) {
    // FileManager 中函数不判断参数的合法性，最好在User中过滤，
    // 暂不考虑过多的参数不合法情况
    if (path.empty()) {
        osstream << "parameter path can't be empty ! \n";
        return false;
    }

    if (path.substr(0, 2) != "..") {
        dirp = path;
    }
    else {
        string pre = curDirPath;
        unsigned int p = 0;
        //可以多重相对路径 .. ../ ../.. ../../
        for (; pre.length() > 3 && p < path.length() && path.substr(p, 2) == ".."; ) {
            pre.pop_back();
            pre.erase(pre.find_last_of('/') + 1);
            p += 2;
            p += p < path.length() && path[p] == '/';
        }
        dirp = pre + path.substr(p);
    }

    if (dirp.length() > 1 && dirp.back() == '/') {
        dirp.pop_back();
    }

    for (unsigned int p = 0, q = 0; p < dirp.length(); p = q + 1) {
        q = dirp.find('/', p);
        q = min(q, (unsigned int)dirp.length());
        if (q - p > DirectoryEntry::DIRSIZ) {
            osstream << "the fileName or dirPath can't be greater than 28 size ! \n";
            return false;
        }
    }
    return true;
}

bool User::GetLastError() {
    if (u_error != U_NOERROR) {
        osstream << "errno = " << u_error;
        Print(u_error);
        u_error = U_NOERROR;
        return true;
    }
    return false;
}

void User::Print(enum ErrorCode err) {
    string estr;
    switch (err) {
    case User::U_NOERROR:
        estr = " No u_error ";
        break;
    case User::U_ENOENT:
        estr = " No such file or directory ";
        break;
    case User::U_EBADF:
        estr = " Bad file number ";
        break;
    case User::U_EACCES:
        estr = " Permission denied ";
        break;
    case User::U_ENOTDIR:
        estr = " Not a directory ";
        break;
    case User::U_ENFILE:
        estr = " File table overflow ";
        break;
    case User::U_EMFILE:
        estr = " Too many open files ";
        break;
    case User::U_EFBIG:
        estr = " File too large ";
        break;
    case User::U_ENOSPC:
        estr = " No space left on device ";
        break;
    default:
        break;
    }
    osstream << estr << endl;
}

short CheckPwd(string username, string password, char* buffer) {
    stringstream ss(buffer);
    short uid;
    string name, pwd;
    while (true)
    {
        ss >> uid >> name >> pwd;
        if (ss.fail())
        {
            return 0;
        }
        if (name== username&& pwd== password)
        {
            return uid;
        }
    }
    return 0;
}


void User::Validate(string username,string password) {
    if (!checkPathName("/etc/passwd")) {
        return;
    }
    int md = FileMode("-r");
    if (md == 0) {
        osstream << "this mode is undefined ! \n";
        return;
    }

    arg[1] = md;
    fileManager->Open();
    if (u_error != U_NOERROR) {
        u_error = U_NOERROR;
        return;
    }

    unsigned int fd = ar0[EAX];
    Seek(to_string(fd), "0", "0");

    int usize = 1024;
    char* buffer = new char[usize+1];
    arg[0] = fd;
    arg[1] = (long)buffer;
    arg[2] = usize;
    fileManager->Read();
    if (GetLastError())
        return;
    Close(to_string(fd));

    buffer[ar0[User::EAX]] = '\0';
    short uid = CheckPwd(username, password, buffer);
    delete[]buffer;

    if (uid==0)
    {
        u_error = User::U_EACCES;
    }
    i_uid = uid;
    return;
}

short AddPwd(string username, string password, char* buffer) {
    stringstream in(buffer),out;
    short uid,max_uid=1,user_id=0;
    string name, pwd;
    while (true)
    {
        in >> uid >> name >> pwd;
        if (in.fail())
        {
            if (user_id == 0) {
                user_id = max_uid + 1;
                out << user_id << ' ' << username << ' ' << password << '\n';
                break;
            }
        }
        else if(name== username)
        {
            max_uid = uid;
            user_id = max_uid;
            out << uid << ' ' << name << ' ' << password << '\n';
        }
        else
        {
            max_uid = uid;
            out << uid << ' ' << name << ' ' << pwd<<'\n';
        }
    }
    strcpy(buffer, out.str().c_str());
    return user_id;
}

void User::AddUser(string username, string password) {
    if (i_uid != 1) {
        u_error = User::U_EACCES;
        GetLastError();
        return;
    }

    Open("/etc/passwd", "-rw");
    unsigned int fd = ar0[EAX];

    int usize = 1024;
    char* buffer = new char[usize + 1];
    Seek(to_string(fd), "0", "0");
    arg[0] = fd;
    arg[1] = (long)buffer;
    arg[2] = usize;
    fileManager->Read();
    if (GetLastError())
        return;

    buffer[ar0[User::EAX]] = '\0';
    short user_id=AddPwd(username, password, buffer);
    Seek(to_string(fd), "0", "0");
    Write(to_string(fd), "", buffer);
    Close(to_string(fd));

    delete[]buffer;
    if (GetLastError())
        return;

    //i_uid = user_id;
    osstream.clear();
    osstream.str("adduser success!\n");
    return;
}

void User::Chmod(string fileName, string mode) {
    stringstream ss(mode);
    unsigned int f_mode;
    ss >> oct >> f_mode;
    f_mode &= 0777;
    if (!checkPathName(fileName)) {
        return;
    }

    //arg[1] = f_mode;
    fileManager->Chmod(f_mode);
    if (GetLastError())
        return;
    osstream << "chmod success" << endl;
}
