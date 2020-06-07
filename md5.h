#ifndef MD5_H
#define MD5_H
#include<iostream>
#include<vector>
#include<string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<memory.h>
#include<unordered_map>
#define GROUP_SIZE 512/8
#define ERROR -1
#define SUCCESS 0
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::unordered_map;
using std::string;
class md5;
typedef uint32_t(md5::*pOperation)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
typedef int Md5Type;
enum{
    MD5_STRING=255,
    MD5_FILE
};
class md5
{
public:
    md5();
    string GenMd5DigestFromFile(const char * FileName);
    string GenMd5DigestFromString(const char* StringMessage,uint64_t MessageSize);
    ~md5();
private:
    int MakeMD5Message(char *Buffer,uint32_t Size);
    string GenMd5Digest();
    void FreeInputContent();
    uint32_t F(uint32_t x,uint32_t y,uint32_t z);
    uint32_t G(uint32_t x,uint32_t y,uint32_t z);
    uint32_t H(uint32_t x,uint32_t y,uint32_t z);
    uint32_t I(uint32_t x,uint32_t y,uint32_t z);
    uint32_t FF(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint32_t s,uint32_t t);
    uint32_t GG(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint32_t s,uint32_t t);
    uint32_t HH(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint32_t s,uint32_t t);
    uint32_t II(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint32_t s,uint32_t t);
    uint32_t RotateLeft(uint32_t x,uint32_t n);
    vector<char *> InputContent;
    unordered_map<int,pOperation> Md5Operation;
    Md5Type LastDigestType;
    uint32_t ResultArray[4]={0x67452301,0xefcdab89,0x98badcfe,0x10325476};
    const uint32_t MagicNum[4]={0x67452301,0xefcdab89,0x98badcfe,0x10325476};
    const uint32_t ConstTablesT[4][16]=
    {
        {
            0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
            0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
            0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
            0x6b901122,0xfd987193,0xa679438e,0x49b40821
        },
        {
            0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
            0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
            0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
            0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a
        },
        {
            0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
            0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
            0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
            0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665
        },
        {
            0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
            0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
            0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
            0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
        }
    };
    const uint32_t ShiftBits[4][4]=
    {
        {7,12,17,22},
        {5,9,14,20},
        {4,11,16,23},
        {6,10,15,21}
    };
    const uint32_t SubGroupNum[4][16]=
    {
        {
            0,1,2,3,
            4,5,6,7,
            8,9,10,11,
            12,13,14,15
        },
        {
            1,6,11,0,
            5,10,15,4,
            9,14,3,8,
            13,2,7,12
        },
        {
            5,8,11,14,
            1,4,7,10,
            13,0,3,6,
            9,12,15,2
        },
        {
            0,7,14,5,
            12,3,10,1,
            8,15,6,13,
            4,11,2,9
        }
    };
};
#endif