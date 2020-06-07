#include"md5.h"
#include<endian.h>
#include<assert.h>
md5::md5()
{
    InputContent.clear();
    Md5Operation[0]=&md5::FF;
    Md5Operation[1]=&md5::GG;
    Md5Operation[2]=&md5::HH;
    Md5Operation[3]=&md5::II;
}
void md5::FreeInputContent()
{
    if(LastDigestType==MD5_FILE){
        for(int i=0;i<InputContent.size();++i){
            delete []InputContent[i];
        }
    }
    else if(LastDigestType==MD5_STRING){
        delete []InputContent[0];
    }
    InputContent.clear();
}
string md5::GenMd5DigestFromString(const char* StringMessage,uint64_t MessageSize)
{
    int iRet=SUCCESS;
    string Md5Digest;
    FreeInputContent();
    LastDigestType=MD5_STRING;
    char *Message=new char[MessageSize+64]; //多分配一个块
    do
    {
        if(StringMessage==NULL || MessageSize<0){
            Md5Digest="";
            break;
        }
        strcpy(Message,StringMessage);
        int64_t RemainingSize=MessageSize;
        uint64_t MessagePointer=0;
        while (RemainingSize>=0){
            if(!RemainingSize){
                if(!(MessageSize % 64)){
                    iRet=MakeMD5Message(NULL,0);
                    if(iRet<0){
                        cerr<<"make md5 error"<<endl;
                        Md5Digest="";
                        goto end;
                    }
                }
                break;
            }
            uint64_t BlockSize=(RemainingSize-64)>=0 ? 64 : RemainingSize;
            iRet=MakeMD5Message(Message+MessagePointer,BlockSize);
            MessagePointer+=BlockSize;
            if(iRet<0){
                cerr<<"make md5 error"<<endl;
                Md5Digest="";
                goto end;
            }
            RemainingSize-=64;
            if(RemainingSize<0){
                RemainingSize=0;
            }
        }
        Md5Digest=GenMd5Digest();
    } while (false);
end:
    if(Md5Digest.empty()){
        delete []Message;
    }
    return Md5Digest;
}
string md5::GenMd5DigestFromFile(const char * FileName)
{
    int iRet=SUCCESS;
    string Md5Digest;
    FreeInputContent();
    LastDigestType=MD5_FILE;
    do
    {
        int Fd=open(FileName,O_RDONLY);
        if(Fd<0){
            perror("File open error:");
            Md5Digest="";
            break;
        }
        fcntl(Fd,O_NONBLOCK);
        int Size=0,MessageSize=0;
        char *ReadBuffer=new char[GROUP_SIZE];
        while((Size=read(Fd,ReadBuffer,64))>=0){
            if(Size){
                iRet=MakeMD5Message(ReadBuffer,Size);
                MessageSize+=Size;
                if(iRet<0){
                    cerr<<"make md5 error"<<endl;
                    Md5Digest="";
                    goto end;
                }
                ReadBuffer=new char[GROUP_SIZE];
            }
            else{
                delete []ReadBuffer;
                if(!(MessageSize % 64)){
                    iRet=MakeMD5Message(NULL,Size);
                    if(iRet<0){
                        cerr<<"make md5 error"<<endl;
                        Md5Digest="";
                        goto end;
                    }
                }
                break;
            }
        }
        Md5Digest=GenMd5Digest();
    } while (false);
end:
    return Md5Digest;
}
int md5::MakeMD5Message(char *Buffer,uint32_t Size)
{
    int iRet=SUCCESS;
    do
    {
        if(Buffer==NULL && Size){
            iRet=ERROR;
            break;
        }
        if(Size==64){
            InputContent.push_back(Buffer);
        }
        else if(!Size){ //传入Size为0,说明原信息正好可以被划分成整数个512的块
            char *PaddingMessage=new char[GROUP_SIZE];
            uint64_t MessageSize=InputContent.size()*512;
            memset(PaddingMessage,0,GROUP_SIZE);
            PaddingMessage[0]=(uint8_t)128;
            memcpy(&PaddingMessage[56],&MessageSize,sizeof(MessageSize));
            InputContent.push_back(PaddingMessage);
        }
        else if(Size*8<448){//读取的小于448，那就不用增加一个新块
            Buffer[Size]=(uint8_t)128;
            uint64_t Length=InputContent.size()*512+Size*8;
            memset(&Buffer[Size+1],0,55-Size);
            memcpy(&Buffer[56],&Length,sizeof(Length));
            InputContent.push_back(Buffer);
        }
        else if(Size*8>=448 && Size<64){//读取的大于448，需要增加一个新块
            Buffer[Size]=(uint8_t)128;
            uint64_t MessageSize=InputContent.size()*512+Size*8;
            if((Size+1)*8==512){ //填一个128就满了
                InputContent.push_back(Buffer);
            }
            else{ //没满
                memset(&Buffer[Size+1],0,64-Size-1);
                InputContent.push_back(Buffer);
            }//当前块填充完成，需要增加一个新块
            char *PaddingMessage=new char[GROUP_SIZE];
            memset(PaddingMessage,0,GROUP_SIZE);
            memcpy(&PaddingMessage[56],&MessageSize,sizeof(MessageSize));
            InputContent.push_back(PaddingMessage);
        }
        else{
            iRet=ERROR;
            break;
        }
    } while (false);
    return iRet;
}
string md5::GenMd5Digest()
{
    uint64_t T1=0,T2=0;
    if(InputContent.empty()){
        return "";
    }
    for(int i=0;i<4;++i){
        ResultArray[i]=MagicNum[i];
    }
    for(int i=0;i<InputContent.size();++i){
        uint32_t *SubGroup=(uint32_t *)InputContent[i];
        uint32_t PreResultArray[4];
        for(int k=0;k<4;++k){
            PreResultArray[k]=ResultArray[k];
        }
        for(int j=0;j<4;++j){
            for(int k=0;k<4;++k){
                ResultArray[0]=(this->*(Md5Operation[j]))(ResultArray[0],ResultArray[1],ResultArray[2],ResultArray[3],SubGroup[SubGroupNum[j][4*k]],ShiftBits[j][0],ConstTablesT[j][4*k]);
                ResultArray[3]=(this->*(Md5Operation[j]))(ResultArray[3],ResultArray[0],ResultArray[1],ResultArray[2],SubGroup[SubGroupNum[j][4*k+1]],ShiftBits[j][1],ConstTablesT[j][4*k+1]);
                ResultArray[2]=(this->*(Md5Operation[j]))(ResultArray[2],ResultArray[3],ResultArray[0],ResultArray[1],SubGroup[SubGroupNum[j][4*k+2]],ShiftBits[j][2],ConstTablesT[j][4*k+2]);
                ResultArray[1]=(this->*(Md5Operation[j]))(ResultArray[1],ResultArray[2],ResultArray[3],ResultArray[0],SubGroup[SubGroupNum[j][4*k+3]],ShiftBits[j][3],ConstTablesT[j][4*k+3]);
            }
        }
        for(int k=0;k<4;++k){
           ResultArray[k]+=PreResultArray[k];
        }
    }
    char StrT1[17],StrT2[17];
    string MD5Digest;
    T1=((T1+htobe32(ResultArray[0]))<<32)+htobe32(ResultArray[1]);
    T2=((T2+htobe32(ResultArray[2]))<<32)+htobe32(ResultArray[3]);
    sprintf(StrT1,"%016lx",T1);
    sprintf(StrT2,"%016lx",T2);
    MD5Digest+=StrT1;
    MD5Digest+=StrT2;
    return MD5Digest;
}
uint32_t md5::F(uint32_t x,uint32_t y,uint32_t z)
{
    return (x & y) | ((~x) & z);
}
uint32_t md5::G(uint32_t x,uint32_t y,uint32_t z)
{
     return (x & z) | (y & (~z));
}
uint32_t md5::H(uint32_t x,uint32_t y,uint32_t z)
{
    return x ^ y ^ z;
}
uint32_t md5::I(uint32_t x,uint32_t y,uint32_t z)
{
    return y ^ (x | (~z));
}
uint32_t md5::FF(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint32_t s,uint32_t t)
{
    return b+RotateLeft(a+F(b,c,d)+x+t,s);
}
uint32_t md5::GG(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint32_t s,uint32_t t)
{
    return b+RotateLeft(a+G(b,c,d)+x+t,s);
}
uint32_t md5::HH(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint32_t s,uint32_t t)
{
    return b+RotateLeft(a+H(b,c,d)+x+t,s);
}
uint32_t md5::II(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint32_t s,uint32_t t)
{
    return b+RotateLeft(a+I(b,c,d)+x+t,s);
}
uint32_t md5::RotateLeft(uint32_t x,uint32_t n)
{
    return (((x) << (n)) | ((x) >> (32-(n)))) ;
}
md5::~md5()
{
    FreeInputContent();
}
