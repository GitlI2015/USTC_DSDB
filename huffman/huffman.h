#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILE_NAME_LENGTH 1024
#define MAX_DIR_NAME_LENGTH 1024
#define BUFFER_SIZE 2048
#define WIDTH 1
typedef char FileName[1024];

struct FileInfo
{
    FILE *FilePointer;
    unsigned int FileSize;
    FileName Name;
    struct stat StatBuf;
    struct FileInfo *Next;
};
struct FileList
{
    struct FileInfo *First;
    struct FileInfo *(*New)();
    unsigned short FileNum;
    size_t TotalFileSize;
};

struct HuffmanTree
{
    unsigned short NodesNum;
    struct HuffmanNode *root;
};
struct HuffmanNode
{
    unsigned char Value;
    unsigned int Number;
    unsigned int Code;
    unsigned short CodeBitLoc;  // specify which bit of the block is to be set
    struct HuffmanNode *left, *right;
};
int NodeCompare(const void *, const void *);
void SetHuffmanCode(struct HuffmanNode *);
double HuffmanTreeStat(struct HuffmanNode *);
int WriteCompressedFile(FILE *, struct FileList *, struct HuffmanTree *,
                        struct HuffmanNode **);
void StoreHuffmanTreeNodes(struct HuffmanNode *, FILE *);

void PrintInfo();
struct FileList *FileList();
struct FileInfo *FileInfo();
int DecodeFile(struct FileList);
int InsertFileInfo(struct FileList *, struct FileInfo *);
int EncodeWithHuffmanTree(struct HuffmanTree *, struct FileList *);
size_t GetExecutionPath(char *processdir, size_t len);
struct HuffmanTree *BuildHuffmanTree(struct FileList *, struct HuffmanNode **);

void GenerateDotFile(struct HuffmanNode *T);
void SubGenerateDot(struct HuffmanNode *n, FILE *fp);
void ListInformation(FILE *);
void DecompressFile(FILE *);
struct HuffmanNode*RestoreHuffmanTree(struct HuffmanNode **,int);
#endif
