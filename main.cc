#include"md5.h"
#include<algorithm>
using std::for_each;
using std::cin;
#define FAILURE 1
typedef int(*AppOperation)(int,char**);
int PrintHelpInfo(int argc,char **argv)
{
    unordered_map<string,string> HelpInfo;
    HelpInfo["[-h]"]="--help information";
    HelpInfo["[-t]"]="--test MD5 application";
    HelpInfo["[-c]"]="[file path of the file computed]\n\t--compute MD5 of the given file";
    HelpInfo["[-v]"]="[file path of the file validated]\n\t--validate the integrality of a given file by manual input MD5 value";
    HelpInfo["[-f]"]="[file path of the file validated]  [file path of the .md5 file]\n\t--validate the integrality of a given file by read MD5 value from .md5 file";
    cout<<"MD5:usage:"<<endl;
    for(auto it=HelpInfo.begin();it!=HelpInfo.end();++it){
        cout<<it->first<<"\t"<<it->second<<endl;
    }
    return SUCCESS;
}
int TestApplication(int argc,char **argv)
{
    vector<string> TestContent={"","a","abc","message digest","abcdefghijklmnopqrstuvwxyz",
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
                                "12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
    md5 *MD5Model=new md5();
    for_each(TestContent.begin(),TestContent.end(),[=](const string& TestString){
        string Md5Digest=MD5Model->GenMd5DigestFromString(TestString.c_str(),TestString.length());
        if(Md5Digest.empty()){
            cerr<<"TestApplication error at string:"<<TestString<<endl;
            exit(EXIT_FAILURE);
        }
        cout<<"MD5(\""<<TestString<<"\")="<<Md5Digest<<endl;
    });
    delete MD5Model;
    return SUCCESS;
}
int GenFileDigest(int argc,char **argv)
{
    if(argc!=3){
        cerr<<"Invalid args"<<endl;
        return FAILURE;
    }
    md5 *MD5Model=new md5();
    string FileDigest=MD5Model->GenMd5DigestFromFile(argv[2]);
    if(FileDigest.empty()){
        cerr<<"GenFileDigest ERROR"<<endl;
        return FAILURE;
    }
    cout<<"The MD5 value of file(\""<<argv[2]<<"\") is:"<<FileDigest<<endl;
    delete MD5Model;
    return SUCCESS;
}
int TestFileByUserInput(int argc,char **argv)
{
    if(argc!=3){
        cerr<<"Invalid args!"<<endl;
        return FAILURE;
    }
    md5 *MD5Model=new md5();
    string FileDigest=MD5Model->GenMd5DigestFromFile(argv[2]);
    if(FileDigest.empty()){
        cerr<<"GenFileDigest ERROR"<<endl;
        return FAILURE;
    }
    string UserInputDigest;
    cout<<"Please input the MD5 value of file(\""<<argv[2]<<"\")..."<<endl;
    cin>>UserInputDigest;
    cout<<"The old MD5 value of file(\""<<argv[2]<<"\") you have input is"<<endl<<UserInputDigest<<endl;
    cout<<"The new MD5 value of file(\""<<argv[2]<<"\") that has computed is"<<endl<<FileDigest<<endl;
    if(UserInputDigest!=FileDigest){
        cout<<"Match Error! The file has been modified!"<<endl;
    }
    else{
        cout<<"OK! The file is integrated "<<endl;
    }
    delete MD5Model;
    return SUCCESS;
}
int TestFileByMd5File(int argc,char **argv)
{
    if(argc!=4){
        cerr<<"Invalid args!"<<endl;
        return FAILURE;
    }
    md5 *MD5Model=new md5();
    string FileDigest=MD5Model->GenMd5DigestFromFile(argv[2]);
    if(FileDigest.empty()){
        cerr<<"GenFileDigest ERROR"<<endl;
        return FAILURE;
    }
    int Fd=open(argv[3],O_RDONLY);
    if(Fd<0){
        perror("Open MD5 Digest File Error:");
        return FAILURE;
    }
    fcntl(Fd,O_NONBLOCK);
    char *Md5DigestInFile=new char[33];
    int Size=read(Fd,Md5DigestInFile,32);
    if(Size<32){
        cerr<<"The file contains MD5 Digest may be corrupted"<<endl;
        return FAILURE;
    }
    Md5DigestInFile[32]='\0';
    cout<<"The old MD5 value of file(\""<<argv[2]<<"\") you in "<<argv[3]<<" is"<<endl<<Md5DigestInFile<<endl;
    cout<<"The new MD5 value of file(\""<<argv[2]<<"\") that has computed is"<<endl<<FileDigest<<endl;
    if(!strcmp(Md5DigestInFile,FileDigest.c_str())){
        cout<<"OK! The file is integrated "<<endl;
    }
    else{
        cout<<"Match Error! The file has been modified!"<<endl;
    }
    delete MD5Model;
    delete []Md5DigestInFile;
    return SUCCESS;
}
int main(int argc,char **argv)
{
    if(argc==1){
        cerr<<"MD5:Missing required options,for help informaton about this program,use -h option."<<endl;
        return FAILURE;
    }
    unordered_map<string,AppOperation> MD5AppOperation;
    MD5AppOperation["-h"]=&PrintHelpInfo;
    MD5AppOperation["-t"]=&TestApplication;
    MD5AppOperation["-c"]=&GenFileDigest;
    MD5AppOperation["-v"]=&TestFileByUserInput;
    MD5AppOperation["-f"]=&TestFileByMd5File;
    auto it=MD5AppOperation.find(argv[1]);
    if(it==MD5AppOperation.end()){
        cerr<<"Unknow Operation"<<endl;
        return FAILURE;
    }
    return it->second(argc,argv);
}