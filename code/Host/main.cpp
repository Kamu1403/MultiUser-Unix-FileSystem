#include "global.h"
#include "Session.h"

DeviceDriver g_DeviceDriver;
BufferManager g_BufferManager;
OpenFileTable g_OpenFileTable;
SuperBlock g_SuperBlock;
FileSystem g_FileSystem;
INodeTable g_INodeTable;
FileManager g_FileManager;
User g_User;
stringstream isstream, osstream;
int g_testNo=0, g_runAuto=0;


void showCmd(string command) {

    static string help =
        "Command       :  help \n"
        "Description   :  Help documentation, showing information about built-in commands. \n"
        "Usage         :  help [pattern] \n"
        "Parameter     :  pattern allowed：  \n"
        "                 help\n"
        "                 login\n"
        "                 fformat(root)\n"
        "                 exit\n"
        "                 mkdir\n"
        "                 cd\n"
        "                 ls\n"
        "                 create\n"
        "                 rm\n"
        "                 open\n"
        "                 close\n"
        "                 seek\n"
        "                 write\n"
        "                 read\n"
        "                 chmod\n"
        "                 adduser(root)\n"
        "                 auto\n"
        ;

    static string login =
        "Command       :  login\n"
        "Description   :  user must login first\n"
        "Usage         :  login username password \n"
        ;

    static string fformat =
        "Command       :  fformat(root)\n"
        "Description   :  Format the entire file system and clear all files and directories\n"
        "Usage         :  fformat \n"
        ;

    static string exit =
        "Command       :  exit\n"
        "Description   :  If it exits correctly, the destructor will be called for normal \n"
        "              :  exit. If there is a cache that has not been updated to the disk \n"
        "              :  in memory, it will be updated in time to ensure correctness.\n"
        "Usage         :  exit \n"
        ;

    static string mkdir =
        "Command       :  mkdir\n"
        "Description   :  'mkdir' creates a directory based on the dir directory name. dir will error if the directory already exists.\n"
        "Usage         :  mkdir dir \n"
        "Parameter     :  dir : can be a relative path or an absolute path.\n"
        ;

    static string ls =
        "Command       :  ls\n"
        "Description   :  List information about files in the dir directory (defaults to the current directory).\n"
        "Usage         :  ls [-l] \n"
        "Parameter     :  -l : set verbose \n"
        ;

    static string cd =
        "Command       :  cd\n"
        "Description   :  Change the current working directory. Change the current directory to dir. The default dir is the user default HOME.\n"
        "Usage         :  cd [dir] \n"
        "Parameter     :  dir : can be a relative path or an absolute path.\n"
        ;

    static string create =
        "Command       :  create\n"
        "Description   :  Create a new file with the default file attributes, the file name is 'file'.\n"
        "Usage         :  create file \n"
        ;

    static string rm =
        "Command       :  delete\n"
        "Description   :  Use rm to delete a file specified by name.\n"
        "Usage         :  rm name \n"
        "Parameter     :  name : can contain relative or absolute paths\n"
        ;

    static string open =
        "Command       :  open\n"
        "Description   :  The function open, opens a file. To read and write a file, you must open it first.\n"
        "                 The default is readable and writable, which can be specified by -rw.\n"
        "Usage         :  open file [-rw] \n"
        "Parameter     :  file : can contain relative or absolute paths\n"
        ;

    static string close =
        "Command       :  close\n"
        "Description   :  Close a file through the file descriptor obtained by open. Open files can be closed.\n"
        "Usage         :  close fd \n"
        ;

    static string seek =
        "Command       :  seek\n"
        "Description   :  Modify the position of the read and write pointer through the file descriptor obtained by open.\n"
        "Usage         :  seek fd offset [pattern] \n"
        "Parameter     :  offset : specifies the offset from the starting position of the displacement, which can be positive or negative.\n"
        "                 pattern : specifies the starting position of the displacement, which is 0(SEEK_SET), 1(SEEK_CUR), 2(SEEK_END)\n"
        "                 , and the default is 0(SEEK_SET).\n"
        ;

    static string write =
        "Command       :  write\n"
        "Description   :  Through the file descriptor obtained by open, write the content to an already opened file.\n"
        "                 Or write the contents of the file inFileName.\n"
        "Usage         :  write fd [content] [-i inFileName] size\n"
        "Parameter     :  content : what is written.\n"
        "                 inFileName : Specifies that the written content is the content in the file InFileName\n"
        "                 size : Specify the number of bytes to write\n"
        ;

    static string read =
        "Command       :  read\n"
        "Description   :  Through the file descriptor obtained by open, read an open file, display the content in the shell or write the file.\n"
        "Usage         :  read fd [-o outFileName] size\n"
        "Parameter     :  outFileName : specifies the output file name\n"
        "                 size : Specify the number of bytes to write\n"
        ;

    static string _auto =
        "Command       :  auto\n"
        "Description   :  Perform automated tests. Helps with testing at the beginning of system startup.\n"
        ;

    static string chmod =
        "Command       :  chmod\n"
        "Description   :  Modify the file attribute of the file file to mode\n"
        "Usage         :  chmod file mode\n"
        "Parameter     :  mode : uses octal to specify modified file access permissions\n"
        ;

    static string adduser =
        "Command       :  adduser(root)\n"
        "Description   :  Add a common user named name and enter the user password. Duplicate usernames will perform an update.\n"
        "Usage         :  adduser name password\n"
        ;

    static unordered_map<string, string*>commands({
        { "help", &help },
        { "login", &login },
        { "fformat", &fformat },
        { "exit", &exit },
        { "mkdir", &mkdir },
        { "cd", &cd },
        { "ls", &ls },
        { "create", &create },
        { "rm", &rm },
        { "open", &open },
        { "close", &close },
        { "seek", &seek },
        { "write", &write },
        { "read", &read },
        {"chmod",&chmod},
        {"adduser",&adduser},
        { "auto", &_auto },
        });

    auto it = commands.find(command);
    if (it == commands.end()) {
        osstream << "unix : " << command << " : don't find this command \n";
        return;
    }
    osstream << *it->second;
}

bool autoTest(string& cmd,bool step=true) {
    static const char* commands[] = {
        "cd /home",
        "mkdir testdir",
        "create testfile1",
        "cd ..",
        "create /home/testfile2",
        "cd home",
        "ls -l",
        "chmod testfile1 700",
        "rm /home/testfile2", 
        "ls -l",
        "open testfile1 -rw",
        "write 8 -i testInputFile.txt 50", 
        "seek 8 30 0",
        "read 8 20", 
        "seek 8 30 0",
        "read 8 -o testOutputFile.txt 20", 
        "seek 8 -20 1",
        "read 8 100", 
        "cd testdir",
        "ls",
        "create testpng",
        "open testpng -rw",
        "write 10 -i test.png 50000",
        "seek 10 0 0",
        "read 10 -o testOutput.png 50000",
        "adduser name1 pass1",
        ""
    };
    int nums = sizeof(commands) / sizeof(char*);
    cmd = commands[g_testNo % nums];
    if (step) {
        return ++g_testNo < nums;
    }
    else
    {
        return g_testNo < nums;
    }
}

void parse(const string& cmd, vector<string>& args) {
    args.clear();
    string str;
    unsigned int p, q;
    for (p = 0, q = 0; q < cmd.length(); p = q + 1) {
        q = cmd.find_first_of(" \n", p);
        str = cmd.substr(p, q - p);
        if (!str.empty()) {
            args.push_back(str);
        }
        if (q == string::npos)
            return;
    }
}

const stringstream& mainloop(const string& input, Session* p_Session) {
    g_User = p_Session->s_User;
    g_testNo = p_Session->testNo;
    g_runAuto = p_Session->autoFlag;


    User* user = &g_User;

    string line = "help";
    vector<string> args;
    string cmd, arg1, arg2, arg3;

    /*the end output of a command*/
    osstream.str("");
    osstream.clear();
    /*get new input of a command*/
    isstream.str(input);
    isstream.clear();

    if (g_runAuto) {
        if (autoTest(line)) {
        }
        else {
            g_runAuto = 0;
            line = "";
        }
    }
    else {
        getline(isstream, line);
    }

    /*start executing*/
    if (line == "")
        goto Prompt;

    parse(line, args);
    cmd = args[0];
    arg1 = args.size() > 1 ? args[1] : "";
    arg2 = args.size() > 2 ? args[2] : "";
    arg3 = args.size() > 3 ? args[3] : "";
    if (!p_Session->login) {
        if (cmd == "login") {
            user->Validate(arg1, arg2);
            if(user->u_error == User::U_NOERROR){
                p_Session->login = true;
                user->username = arg1;
                osstream << "login success!" << endl;
            }
            else{
                osstream << "wrong username or password!" << endl;
            }
        }
        else{
            osstream << "use login command to log in." << endl;
        }
    }
    else
    {
        if (cmd == "help") {
            showCmd(arg1.empty() ? "help" : arg1);
        }
        else if (cmd == "fformat") {
            g_OpenFileTable.Format();
            g_INodeTable.Format();
            g_BufferManager.FormatBuffer();
            g_FileSystem.FormatDevice();
            g_INodeTable = INodeTable();
            g_FileManager = FileManager();
            g_User = User();
            g_User.username = "root";
            user->Mkdir("bin");
            user->Mkdir("etc");
            user->Mkdir("home");
            user->Mkdir("dev");
            user->Cd("etc");
            user->Create("passwd", "-rw");
            user->Open("passwd", "-rw");
            unsigned int fd = user->ar0[User::EAX];
            user->Seek(to_string(fd), "0", "0");
            user->Write(to_string(fd), "", "1 root root");
            user->Close(to_string(fd));
            user->Cd("..");
        }
        else if (cmd == "exit") {
            exit(0);
        }
        else if (cmd == "auto") {
            g_runAuto = 1;
            osstream << "autoTest begin ... \njust press enter to continue ... \n";
        }
        else if (cmd == "mkdir") {
            user->Mkdir(args[1]);
        }
        else if (cmd == "ls") {
            if (arg1 == "-l")
                user->Ls(true);
            else
                user->Ls();
        }
        else if (cmd == "cd") {
            user->Cd(arg1);
        }
        else if (cmd == "create") {
            //create 没有需要模式的必要
            user->Create(arg1, "-rw");
        }
        else if (cmd == "rm") {
            user->Delete(arg1);
        }
        else if (cmd == "open") {
            user->Open(arg1, line);
        }
        else if (cmd == "close") {
            user->Close(arg1);
        }
        else if (cmd == "seek") {
            user->Seek(arg1, arg2, arg3);
        }
        else if (cmd == "read") {
            if (arg2 == "-o")
                user->Read(arg1, arg3, args[4]);
            else
                user->Read(arg1, "", arg2);
        }
        else if (cmd == "write") {
            if (arg2 == "-i")
                user->Write(arg1, arg3, args[4]);
            else
                user->Write(arg1, "", arg2);
        }
        else if (cmd == "adduser") {
            user->AddUser(arg1, arg2);
        }
        else if (cmd == "chmod")
        {
            user->Chmod(arg1, arg2);
        }
        else if (cmd != "") {
            osstream << "shell : " << cmd << " : don't find this command \n";
        }
    }

Prompt:
    osstream << "[ "<< user->username <<": " << user->curDirPath << " ]$ ";
        
    if (g_runAuto) {
        if (autoTest(line,false)) {
            osstream << line;
        }
        else {
            osstream << "auto test finished ... \n";
        }
    }

    p_Session->s_User= g_User;
    p_Session->osstream.str(osstream.str());
    p_Session->testNo = g_testNo;
    p_Session->autoFlag = g_runAuto;
    return osstream;
}

