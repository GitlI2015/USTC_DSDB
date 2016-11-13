#include "huffman.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ExecuteDir[MAX_DIR_NAME_LENGTH];
char FullPath[MAX_DIR_NAME_LENGTH];
int GlobalIndex = 0;

int main(int argc, char **argv)
{
    FILE *DestinationFile;
    FileName DestinationFileName;
    struct FileInfo *Temp;
    DestinationFileName[0] = '\0';

    struct FileList *OriginalFiles = FileList();

    enum
    {
        compress,
        decompress,
        list
    } mode;

    // set default mode as compress
    mode = compress;

    if (argc == 1) {
        PrintInfo();
        return 0;
    }

    GetExecutionPath(ExecuteDir, sizeof(ExecuteDir));

    for (int i = 1; i < argc; i++) {
        if (strncmp("-o", argv[i], 2) == 0) {
            if (i + 1 == argc) {
                fprintf(stderr,
                        "huffman: error: missing file name after '-o'\n");
                return -1;
            }
            strcpy(DestinationFileName, argv[i + 1]);
            ++i;
        }
        else if (strncmp("-d", argv[i], 2) == 0)
        {
            if (i + 1 == argc) {
                fprintf(stderr,
                        "huffman: error: missing file name after '-d'\n");
                return -1;
            }
            mode = decompress;
            strcpy(DestinationFileName, argv[i + 1]);
            if(access(DestinationFileName,F_OK) == -1){
                fprintf(stderr, "huffman: error: %s: No Such File\n", DestinationFileName);
                return -1;
            }
            ++i;
        }
        else if (strncmp("-l", argv[i], 2) == 0)
        {
            if (i != 1) {
                fprintf(stderr, "huffman: error: usage: huffman -l [files].\n");
                return -1;
            }
            mode = list;
            strcpy(DestinationFileName, argv[i + 1]);
            if(access(DestinationFileName,F_OK) == -1){
                fprintf(stderr, "huffman: error: %s: No Such File\n", DestinationFileName);
                return -1;
            }
            ++i;
        }
        else
        {
            Temp = OriginalFiles->New();
            strcpy(Temp->Name, argv[i]);
            if (argv[i][0] == '/')
                sprintf(FullPath, "%s", Temp->Name);
            else
                sprintf(FullPath, "%s%s", ExecuteDir, Temp->Name);
            if (access(Temp->Name, F_OK) == -1) {
                fprintf(stderr, "huffman: error: %s: No Such File\n", Temp->Name);
                return -1;
            }
            else
            {
                stat(Temp->Name, &Temp->StatBuf);
                Temp->FileSize = Temp->StatBuf.st_size;
            }
            InsertFileInfo(OriginalFiles, Temp);
        }
    }

    struct HuffmanTree *CodingTree;
    struct HuffmanNode *HuffmanNodeStack[(int)pow(2, 8 * WIDTH)];
    if (mode == list) {
        DestinationFile = fopen(DestinationFileName, "rb");
        ListInformation(DestinationFile);
        fclose(DestinationFile);
    }
    else if (mode == compress)
    {
        CodingTree = BuildHuffmanTree(OriginalFiles, HuffmanNodeStack);
        GenerateDotFile(CodingTree->root);
        // printf("%d", i);

        if (DestinationFileName[0] == '\0')
            sprintf(DestinationFileName, "%s%s", OriginalFiles->First->Name,
                    ".huf");

        DestinationFile = fopen(DestinationFileName, "wb");

        WriteCompressedFile(DestinationFile, OriginalFiles, CodingTree,
                            HuffmanNodeStack);
    }
    else if (mode == decompress)
    {
        DestinationFile = fopen(DestinationFileName, "rb");
        DecompressFile(DestinationFile);
        remove(DestinationFileName);
    }
    return 0;
}
void PrintInfo()
{
    printf("Tiny huffman-tree based compressing tool.\n");
    printf("Written by Gloit 2016.\n");
}
struct FileInfo *FileInfo()
{
    struct FileInfo *fi = (struct FileInfo *)malloc(sizeof(struct FileInfo));
    fi->Next            = NULL;
    fi->FileSize        = 0;
    return fi;
}
struct FileList *FileList()
{
    struct FileList *fl = (struct FileList *)malloc(sizeof(struct FileList));
    fl->FileNum         = 0;
    fl->TotalFileSize   = 0;
    fl->New             = FileInfo;
    fl->First           = NULL;
    return fl;
}
size_t GetExecutionPath(char *processdir, size_t len)
{
    char *path_end;
    int i = readlink("/proc/self/exe", processdir, len);
    if (i <= 0) return -1;
    path_end = strrchr(processdir, '/');
    if (path_end == NULL) return -1;
    i -= path_end - processdir + 1;
    ++path_end;
    *path_end = '\0';
    return (size_t)(path_end - processdir);
}

// this function inserts file information in lexicographical order
int InsertFileInfo(struct FileList *fl, struct FileInfo *fi)
{
    struct FileInfo *t = fl->First;
    struct FileInfo *tt;

    if (t == NULL) {
        fl->First         = fi;
        fl->TotalFileSize = fi->FileSize;
        ++fl->FileNum;
        return 1;
    }

    if (strcmp(t->Name, fi->Name) > 0) {
        fl->First = fi;
        fi->Next  = t;
        fl->TotalFileSize += fi->FileSize;
        ++fl->FileNum;
        return 1;
    }

    while (t->Next != NULL && strcmp(t->Next->Name, fi->Name) < 0) t = t->Next;

    if (t->Next == NULL) {
        t->Next = fi;
        fl->TotalFileSize += fi->FileSize;
        ++fl->FileNum;
        return 1;
    }

    if (strcmp(t->Next->Name, fi->Name) || strcmp(t->Name, fi->Name) == 0)
        return 0;

    tt       = t->Next;
    t->Next  = fi;
    fi->Next = tt;
    fl->TotalFileSize += fi->FileSize;
    ++fl->FileNum;
    return 1;
}
struct HuffmanTree *BuildHuffmanTree(struct FileList *fl,
                                     struct HuffmanNode **Stack)
{
    struct HuffmanTree *tt =
        (struct HuffmanTree *)malloc(sizeof(struct HuffmanTree));
    unsigned char Buffer[BUFFER_SIZE];
    unsigned int Mask = 0;
    unsigned int TemporaryMemory;
    size_t BitNums = pow(2, WIDTH * 8);
    for (unsigned short i = 0; i < WIDTH; i++) {
        Mask = Mask << 8;
        Mask += 0xFF;
    }

    struct HuffmanNode *Nodes;
    Nodes = (struct HuffmanNode *)calloc(BitNums, sizeof(struct HuffmanNode));
    struct HuffmanNode *TStack[(int)BitNums];

    for (int i = 0; i < (int)BitNums; ++i) Nodes[i].Value = i;

    for (struct FileInfo *ti = fl->First; ti != NULL; ti = ti->Next) {
        if (ti->Name[0] == '/')
            sprintf(FullPath, "%s", ti->Name);
        else
            sprintf(FullPath, "%s%s", ExecuteDir, ti->Name);
        ti->FilePointer = fopen(FullPath, "rb");
        while (!feof(ti->FilePointer)) {
            int num =
                fread(Buffer, WIDTH, BUFFER_SIZE / WIDTH, ti->FilePointer);

            for (int i = 0; i < num; i++) {
                TemporaryMemory = Buffer[i];

                ++Nodes[TemporaryMemory].Number;
            }
        }
    }

    for (int i = 0; i < (int)BitNums; ++i) TStack[i] = Stack[i] = Nodes + i;

    qsort(TStack, BitNums, sizeof(struct HuffmanNode *), NodeCompare);

    int NodesNum = 0;
    for (int i = 0; i < (int)BitNums; ++i)
        if (TStack[i]->Number != 0) {
            NodesNum = BitNums - i;
            break;
        }
    tt->NodesNum = NodesNum;

    for (int i = 0; i < NodesNum; ++i)

        TStack[i] = TStack[i + BitNums - NodesNum];


    while (NodesNum > 1) {
        struct HuffmanNode *t =
            (struct HuffmanNode *)calloc(1, sizeof(struct HuffmanNode));
        t->left   = TStack[0];
        t->right  = TStack[1];
        t->Number = TStack[0]->Number + TStack[1]->Number;
        TStack[0] = t;

        int i;  // loop variable
        for (i = 2; i < NodesNum; ++i)
            if (TStack[i]->Number > t->Number) {
                if (i == 2) {
                    TStack[0] = t;
                    for (int j    = 1; j < NodesNum - 1; j++)
                        TStack[j] = TStack[j + 1];

                    t = NULL;
                }
                else
                {
                    for (int j = 0; j < i - 2; j++) TStack[j] = TStack[j + 2];

                    TStack[i - 2] = t;
                    t             = NULL;

                    for (int j    = i - 1; j < NodesNum - 1; ++j)
                        TStack[j] = TStack[j + 1];
                }
                --NodesNum;
                break;
            }

        if (i == NodesNum && t != NULL) {
            for (int j = 0; j < NodesNum - 2; ++j) TStack[j] = TStack[j + 2];

            TStack[NodesNum - 2] = t;
            --NodesNum;
        }
    }

    tt->root = TStack[0];

    SetHuffmanCode(tt->root);

    return tt;
}
int NodeCompare(const void *lp, const void *rp)
{
    return (*((struct HuffmanNode **)lp))->Number -
           (*((struct HuffmanNode **)rp))->Number;
}
void SetHuffmanCode(struct HuffmanNode *n)
{
    if (n->left != NULL) {
        n->left->Code       = n->Code;
        n->left->CodeBitLoc = n->CodeBitLoc + 1;
        SetHuffmanCode(n->left);
    }

    if (n->right != NULL) {
        n->right->Code       = n->Code | ((unsigned int)1 << n->CodeBitLoc);
        n->right->CodeBitLoc = n->CodeBitLoc + 1;
        SetHuffmanCode(n->right);
    }
}
double HuffmanTreeStat(struct HuffmanNode *n)
{
    if (n == NULL) return 0;
    if (n->left == NULL && n->right == NULL)
        return n->Number * n->CodeBitLoc;
    else
        return HuffmanTreeStat(n->left) + HuffmanTreeStat(n->right);
}

void GenerateDotFile(struct HuffmanNode *T)
{
    FILE *fp = fopen("sample.dot", "w+");
    fprintf(fp, "digraph g{\n");
    fprintf(fp, "    node [shape = record,height=.1];\n");
    fprintf(fp, "    node%d[label=\"<f0>|<f1> %u|<f2> \"];\n", GlobalIndex,
            T->Number);
    SubGenerateDot(T, fp);
    fprintf(fp, "}");
    fclose(fp);
    printf("Generate sample.dot successfully!\n");
}
void SubGenerateDot(struct HuffmanNode *n, FILE *fp)
{
    if(n==NULL) return;
    int index = GlobalIndex;
    if (n->left != NULL) {
        fprintf(fp, "    node%d[label=\"<f0>|<f1> %u|<f2> \"];\n",
                ++GlobalIndex, n->left->Number);
        fprintf(fp, "    \"node%d\":f0 -> \"node%d\":f1;\n", index,
                GlobalIndex);
        SubGenerateDot(n->left,fp);
    }
    if (n->right != NULL) {
        fprintf(fp, "    node%d[label=\"<f0>|<f1> %u|<f2> \"];\n",
                ++GlobalIndex, n->right->Number);
        fprintf(fp, "    \"node%d\":f2 -> \"node%d\":f1;\n", index,
                GlobalIndex);
        SubGenerateDot(n->right,fp);
    }
}

int WriteCompressedFile(FILE *fp, struct FileList *fl, struct HuffmanTree *ht,
                        struct HuffmanNode **hs)
{
    /*
     *------------------------------------------
     *|           |              |             |
     *|    EF2D   |   File Num   |  File List  |
     *|  4 bytes  |   2 bytes    |             |
     *|----------------------------------------|
     *| Tree Size |        Huffman Tree        |
     *|----------------------------------------|
     *| Data Size |      Compressed Data       |
     *------------------------------------------
     */
    unsigned char Buffer[BUFFER_SIZE];

    fwrite("\xEF\x2D", 2, 1, fp);
    fwrite(&fl->FileNum, sizeof(short), 1, fp);
    for (struct FileInfo *t = fl->First; t != NULL; t = t->Next) {
        fwrite(t->Name, 1, strlen(t->Name) + 1, fp);
        fwrite(&t->FileSize, 4, 1, fp);  // store file size (bytes)
    }

    fwrite(&ht->NodesNum, 2, 1, fp);
    StoreHuffmanTreeNodes(ht->root, fp);

    double total          = HuffmanTreeStat(ht->root);
    unsigned int DataSize = ceil(total);
    fwrite(&DataSize, 4, 1, fp);

    int BufferI = 0;
    int BufferJ = 0;
    unsigned char Buffer2[BUFFER_SIZE];
    int sum = 0;
    memset(Buffer2, 0, BUFFER_SIZE);
    for (struct FileInfo *t = fl->First; t != NULL; t = t->Next) {
        fseek(t->FilePointer, 0, SEEK_SET);
        int num;
        while (!feof(t->FilePointer)) {
            num = fread(Buffer, 1, BUFFER_SIZE, t->FilePointer);
            sum += num;

            for (int i = 0; i < num; i += 1) {
                struct HuffmanNode *tn = hs[Buffer[i]];
                if (tn->CodeBitLoc + BufferJ < 32) {
                    *(((unsigned int *)Buffer2) + BufferI) |= tn->Code
                                                              << BufferJ;
                    BufferJ += tn->CodeBitLoc;
                }
                else
                {
                    *(((unsigned int *)Buffer2) + BufferI) |= tn->Code
                                                              << BufferJ;
                    ++BufferI;
                    if (BufferI == 512) {
                        fwrite(Buffer2, 1, BUFFER_SIZE, fp);
                        memset(Buffer2, 0, BUFFER_SIZE);
                        BufferI = 0;
                    }
                    *(((unsigned int *)Buffer2) + BufferI) |=
                        tn->Code >> (32 - BufferJ);
                    BufferJ = (tn->CodeBitLoc + BufferJ) % 32;
                }
            }
        }
        if (BufferJ == 0)
            fwrite(Buffer2, 1, BufferI * 4, fp);
        else
            fwrite(Buffer2, 1, BufferI * 4 + ceil(BufferJ / 8.0), fp);
        memset(Buffer2, 0, BUFFER_SIZE);
        BufferI = BufferJ = 0;

        fclose(t->FilePointer);
        remove(t->Name);
    }
    return 0;
}
void StoreHuffmanTreeNodes(struct HuffmanNode *n, FILE *fp)
{
    if (n->left == NULL && n->right == NULL) {
        fwrite(&n->Value, 1, 1, fp);
        fwrite(&n->Code, 4, 1, fp);
        fwrite(&n->CodeBitLoc, sizeof(short), 1, fp);
        return;
    }
    if (n->left != NULL) StoreHuffmanTreeNodes(n->left, fp);
    if (n->right != NULL) StoreHuffmanTreeNodes(n->right, fp);
}
void ListInformation(FILE *fp)
{
    unsigned char Buffer[BUFFER_SIZE];
    unsigned short size;
    unsigned int FileSize;
    fread(Buffer, 2, 1, fp);
    printf("Files in this archive:\n");
    printf("No. \t%-12s%-8s\n", "name", "size");
    if (Buffer[0] == (unsigned char)'\xEF' &&
        Buffer[1] == (unsigned char)'\x2D')
    {
        fread(&size, sizeof(short), 1, fp);
        int i = 0;
        int j = 0;
        while (i < size) {
            fread(Buffer + j, 1, 1, fp);
            if (Buffer[j] == '\0') {
                fread(&FileSize, sizeof(int), 1, fp);
                j = 0;
                printf("%2d\t%-12s%-8u\n", i + 1, Buffer, FileSize);
                ++i;
            }
            else
                ++j;
        }
    }
    else
    {
        fprintf(stderr,
                "huffman: error: Head of file signature not found. This file "
                "is either non-huffmanized file or broken\n");
        fclose(fp);
        exit(-1);
    }
}

void DecompressFile(FILE *fp)
{
    unsigned char Buffer[BUFFER_SIZE];
    unsigned short size;
    unsigned short NodeNum;

    fread(Buffer, 2, 1, fp);
    if (Buffer[0] != (unsigned char)'\xEF' ||
        Buffer[1] != (unsigned char)'\x2D')
    {
        fprintf(stderr,
                "huffman: error: Head of file signature not found. This file "
                "is either non-huffmanized file or broken\n");
        fclose(fp);
        exit(-1);
    }

    fread(&size, sizeof(short), 1, fp);
    unsigned int FileSize[size];
    unsigned char FileName[size][BUFFER_SIZE];

    {
        int i = 0;
        int j = 0;
        while (i < size) {
            fread(FileName[i] + j, 1, 1, fp);
            if (FileName[i][j] == '\0') {
                fread(&FileSize[i], sizeof(int), 1, fp);
                j = 0;
                ++i;
            }
            else
                ++j;
        }
    }

    fread(&NodeNum, sizeof(short), 1, fp);

    struct HuffmanNode *Nodes[NodeNum];
    struct HuffmanNode *root;

    for (int i = 0; i < NodeNum; ++i) {
        struct HuffmanNode *tn =
            (struct HuffmanNode *)malloc(sizeof(struct HuffmanNode));
        Nodes[i] = tn;

        fread(&tn->Value, 1, 1, fp);
        fread(&tn->Code, 4, 1, fp);
        fread(&tn->CodeBitLoc, sizeof(short), 1, fp);
    }

    //建树可能出现问题
    root = RestoreHuffmanTree(Nodes, NodeNum);

    // read data size block which may not be used
    fread(Buffer, sizeof(int), 1, fp);

    int num = fread(Buffer, 1, BUFFER_SIZE, fp);
    int sum = num;

    unsigned int CompressedCode;
    unsigned short BitLoc = 0;

    unsigned char WriteInBuffer[BUFFER_SIZE];
    unsigned short WriteInBufferSub = 0;

    CompressedCode = *((unsigned int *)Buffer);
    int k          = 0;
    for (int i = 0; i < size; ++i) {
        FILE *tfp              = fopen(FileName[i], "wb");
        int j                  = 0;
        struct HuffmanNode *tn = root;
        while (j < FileSize[i]) {
            while (tn->left != NULL || tn->right != NULL) {
                if (CompressedCode >> BitLoc & 1)
                    tn = tn->right;
                else
                    tn = tn->left;
                BitLoc++;
                if (BitLoc == 32) {
                    ++k;
                    CompressedCode = *((unsigned int *)Buffer + k);
                    BitLoc         = 0;
                }
                if (4 * (k + 1) == num && !feof(fp)) {
                    if (!feof(fp)) num = fread(Buffer, 1, BUFFER_SIZE, fp);
                    sum += num;
                    k = -1;
                }
            }
            WriteInBuffer[WriteInBufferSub++] = tn->Value;
            //fwrite(&tn->Value, sizeof(char), 1, tfp);
            if(WriteInBufferSub==BUFFER_SIZE)
            {
                fwrite(WriteInBuffer,1,BUFFER_SIZE,tfp);
                WriteInBufferSub = 0;
            }
            tn = root;
            ++j;
        }
        fwrite(WriteInBuffer,1,WriteInBufferSub,tfp);
        WriteInBufferSub = 0;
        fclose(tfp);
    }
}

struct HuffmanNode *RestoreHuffmanTree(struct HuffmanNode **Nodes, int TreeSize)
{
    struct HuffmanNode *r =
        (struct HuffmanNode *)malloc(sizeof(struct HuffmanNode));
    struct HuffmanNode *t = r;
    unsigned int Code;
    for (int i = 0; i < TreeSize; ++i) {
        t    = r;
        Code = Nodes[i]->Code;
        for (int j = 0; j < Nodes[i]->CodeBitLoc; ++j) {
            if (Code & 1) {
                if (t->right == NULL) {
                    t->right = (struct HuffmanNode *)malloc(
                        sizeof(struct HuffmanNode));
                    t->right->left = t->right->right = NULL;
                }
                if (j == Nodes[i]->CodeBitLoc - 1)
                    t->right->Value = Nodes[i]->Value;
                else
                    t = t->right;
            }
            else
            {
                if (t->left == NULL) {
                    t->left = (struct HuffmanNode *)malloc(
                        sizeof(struct HuffmanNode));
                    t->left->left = t->left->right = NULL;
                }
                if (j == Nodes[i]->CodeBitLoc - 1)
                    t->left->Value = Nodes[i]->Value;
                else
                    t = t->left;
            }
            Code = Code >> 1;
        }
    }

    return r;
}
