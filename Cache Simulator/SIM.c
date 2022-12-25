// cristian gutierrez
// COMP ARCH SUBOH SUBOH

// header files
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct hexAdd
{
    char operation;
    char address[64];
    long index;
    long long int tag;
};

struct cacheDetails
{
    char *filePath;
    int size;
    int numIndex;
    int numOfSets;
    int RP;
    int WP;
    int offset;
};

struct dataMask
{
    int offset;
    int index;
    long long int tag;
    int blockOffsetBits;
    int numIndexBits;
    int numTagBits;
};

struct Block
{
    int dirty;
    long long int address;
};

// global vars
char currOp;
bool inCache, filled;
long long int hits;
long long int writes;
long long int misses;
long long int reads;

// function prototypes

// utility functions
void printOutput();
void getMasks(struct dataMask *dataMask, struct cacheDetails *details);
void freeMainCache(struct Block **mainCache, struct cacheDetails *details);
void freeRP_Cache(unsigned long long **rpCache, struct cacheDetails *details);
void getCurrentIndexAndTag(struct dataMask *dataMask, struct hexAdd *address, struct Block *Line, struct cacheDetails *details);
void freeEverything(unsigned long long **rpCache, struct cacheDetails details, struct Block **mainCache);

// essential functions
void declareCache(struct Block **mainCache, struct cacheDetails *details);
struct cacheDetails processArguments(int numArgs, char *arguments[], struct cacheDetails details);
bool hitOrMiss(struct cacheDetails details, struct hexAdd address, struct Block Line, struct Block **mainCache);
void declareReplacementPolicyStoring(unsigned long long **rpCache, struct cacheDetails *details);
void initCaches(struct Block **mainCache, unsigned long long **rpCache, struct cacheDetails details);
unsigned long long **updateLRU(struct cacheDetails details, struct hexAdd address, struct Block Line, unsigned long long **rpCache);
unsigned long long **updateFIFO(struct cacheDetails details, struct hexAdd address, struct Block Line, unsigned long long **rpCache);
struct Block **fillCache(struct Block **mainCache, struct cacheDetails details, struct hexAdd address, struct Block Line, unsigned long long **rpCache);
void runSim(char lineString[], struct hexAdd address, struct Block Line, struct cacheDetails details, struct dataMask dataMask, unsigned long long **rpCache, struct Block **mainCache);

int main(int numArgs, char *arguments[])
{
    // all variables
    char lineString[64];
    struct dataMask dataMask;
    struct hexAdd address;
    struct Block Line;
    struct cacheDetails details = processArguments(numArgs, arguments, details);
    unsigned long long **rpCache = (unsigned long long **)malloc((int)details.numOfSets * sizeof(unsigned long long *));
    struct Block **mainCache = (struct Block **)malloc((int)details.numOfSets * sizeof(struct Block *));

    // setting up caches and future vals
    initCaches(mainCache, rpCache, details);

    // running simulation
    FILE *file = fopen(details.filePath, "r");
    while (fgets(lineString, 64, file) != NULL)
        runSim(lineString, address, Line, details, dataMask, rpCache, mainCache);
    fclose(file);

    // Ta Da!
    printOutput();

    // free our dynamic memory
    freeEverything(rpCache, details, mainCache);
}

void declareCache(struct Block **mainCache, struct cacheDetails *details)
{
    int counter1 = details->numIndex;
    int counter2 = details->numOfSets;

    while (counter1 != 0)
    {
        while (counter2 != 0)
        {
            mainCache[counter2][counter1].address = 0;
            counter2--;
        }
        counter1--;
    }
}

void declareReplacementPolicyStoring(unsigned long long **rpCache, struct cacheDetails *details)
{
    int counter = details->numIndex;
    int counter2 = details->numOfSets;
    while (counter != 0)
    {

        while (counter2 != 0)
        {

            rpCache[counter2][counter] = 0;
            counter2--;
        }
        counter--;
    }
}

void freeMainCache(struct Block **mainCache, struct cacheDetails *details)
{
    int counter = details->numOfSets;
    while (counter != 0)
    {
        free(mainCache[counter]);
        counter--;
    }
    free(mainCache);
}

void freeRP_Cache(unsigned long long **rpCache, struct cacheDetails *details)
{
    int counter = details->numOfSets;

    while (counter != 0)
    {
        free(rpCache[counter]);
        counter--;
    }
    free(rpCache);
}

// function for dataMask generation
void getMasks(struct dataMask *dataMask, struct cacheDetails *details)
{
    dataMask->offset = (details->offset) - 1;
    dataMask->blockOffsetBits = log2((dataMask->offset) + 1);
    dataMask->index = ((details->numIndex) - 1) << (dataMask->blockOffsetBits);
    dataMask->numIndexBits = log2(details->numIndex);
    dataMask->numTagBits = (-1 * ((dataMask->blockOffsetBits) + (dataMask->numIndexBits))) + -64;
    dataMask->tag = ((int)pow(2, (dataMask->numTagBits)) - 1) << ((dataMask->blockOffsetBits) + (dataMask->numIndexBits));
}

// function for current index and tag
void getCurrentIndexAndTag(struct dataMask *dataMask, struct hexAdd *address, struct Block *Line, struct cacheDetails *details)
{
    // shift right
    address->tag = ((dataMask->tag) & (Line->address)) >> ((dataMask->blockOffsetBits) + (dataMask->numIndexBits));
    address->index = ((dataMask->index) & (Line->address)) >> dataMask->blockOffsetBits;
}

struct cacheDetails processArguments(int numArgs, char *arguments[], struct cacheDetails details)
{
    // allocate space for file path
    details.filePath = (char *)malloc(sizeof(arguments[5]) * sizeof(char));
    // store arguments
    details.size = atoi(arguments[1]);
    details.numOfSets = atoi(arguments[2]);
    details.RP = atoi(arguments[3]);
    details.WP = atoi(arguments[4]);
    strcpy(details.filePath, arguments[5]);
    details.offset = 64;
    details.numIndex = details.size / ((details.offset) * (details.numOfSets));

    return details;
}

void freeEverything(unsigned long long **rpCache, struct cacheDetails details, struct Block **mainCache)
{
    freeRP_Cache(rpCache, &details);

    freeMainCache(mainCache, &details);

    free(details.filePath);
}

void printOutput()
{
    unsigned long long totalHitsAndMisses = hits + misses;
    double missRatio = misses / (totalHitsAndMisses * 1.0);
    printf("Miss Ratio %f\nWrite %lld\nRead %lld\n", missRatio, writes, reads);
}

void initCaches(struct Block **mainCache, unsigned long long **rpCache, struct cacheDetails details)
{
    int i;

    for (i = 0; i < details.numOfSets; ++i)
        mainCache[i] = (struct Block *)malloc((int)details.numIndex * sizeof(struct Block));

    for (i = 0; i < details.numOfSets; ++i)
        rpCache[i] = (unsigned long long *)malloc((int)details.numIndex * sizeof(unsigned long long));

    for (int i = 0; i < (details.numIndex); i++)
        for (int j = 0; j < (details.numOfSets); j++)
            rpCache[j][i] = 0;

    for (int i = 0; i < (details.numIndex); i++)
    {
        for (int j = 0; j < (details.numOfSets); j++)
        {
            mainCache[j][i].address = 0;
        }
    }
}

bool hitOrMiss(struct cacheDetails details, struct hexAdd address, struct Block Line, struct Block **mainCache)
{
    for (int i = 0; i < (details.numOfSets); i++)
    {
        if (mainCache[i][address.index].address == address.tag)
        {
            hits++;
            inCache = true;
            if (Line.dirty == 1)
            {
                mainCache[i][address.index].dirty = 1;
            }
            break;
        }
        else
            inCache = false;
    }

    return inCache;
}

struct Block **fillCache(struct Block **mainCache, struct cacheDetails details, struct hexAdd address, struct Block Line, unsigned long long **rpCache)
{
    misses++;
    reads++;

    int flag = 0;
    for (int i = 0; i < (details.numOfSets); i++)
    {
        if (flag)
            break;

        else if (mainCache[i][address.index].address == 0)
        {
            flag = 1;
            filled = true;
            mainCache[i][address.index].address = address.tag;
            mainCache[i][address.index].dirty = Line.dirty;
        }
    }
    if (!filled)
    {
        for (int i = 0; i < (details.numOfSets); i++)
        {
            if (mainCache[i][address.index].address == rpCache[details.numOfSets - 1][address.index])
            {
                // implements dirty eviction check
                if ((mainCache[i][address.index].dirty == 1) && (mainCache[i][address.index].address != address.tag))
                {
                    writes++;
                }
                mainCache[i][address.index].address = address.tag;
                mainCache[i][address.index].dirty = Line.dirty;
                filled = true;
            }
        }
    }
    filled = false;
    return mainCache;
}

unsigned long long **updateLRU(struct cacheDetails details, struct hexAdd address, struct Block Line, unsigned long long **rpCache)
{
    int replacementLocation;

    // Case for LRU Storing
    if (inCache)
    {
        for (int i = 0; i < (details.numOfSets); i++)
        {
            if (rpCache[i][address.index] == address.tag)
            {
                replacementLocation = i;
                break;
            }
        }
        for (int j = replacementLocation; j >= 0; j--)
        {
            if (j == 0)
            {
                rpCache[j][address.index] = address.tag;
                break;
            }
            rpCache[j][address.index] = rpCache[j - 1][address.index];
        }
    }
    else
    {
        for (int j = (details.numOfSets - 1); j >= 0; j--)
        {
            if (j == 0)
            {
                rpCache[j][address.index] = address.tag;
                break;
            }
            else
                rpCache[j][address.index] = rpCache[j - 1][address.index];
        }
    }

    return rpCache;
}

unsigned long long **updateFIFO(struct cacheDetails details, struct hexAdd address, struct Block Line, unsigned long long **rpCache)
{

    // case for FIFO storing
    if (!inCache)
    {
        for (int j = (details.numOfSets - 1); j >= 0; j--)
        {
            if (j == 0)
                rpCache[j][address.index] = address.tag;
            else
                rpCache[j][address.index] = rpCache[j - 1][address.index];
        }
    }

    return rpCache;
}

void runSim(char lineString[], struct hexAdd address, struct Block Line, struct cacheDetails details, struct dataMask dataMask, unsigned long long **rpCache, struct Block **mainCache)
{

    sscanf(lineString, "%c %s", &address.operation, &address.address);
    currOp = toupper(address.operation);
    Line.address = strtoll(address.address, NULL, 16);

    getMasks(&dataMask, &details);

    // Sets the dirty bit if its write back as well as counts the writes for write through
    if ((currOp == 'W') && details.WP == 1)
    {
        Line.dirty = 1;
    }
    else if (currOp == 'W')
    {
        Line.dirty = 0;
        writes++;
    }
    else
        Line.dirty = 0;

    getCurrentIndexAndTag(&dataMask, &address, &Line, &details);
    inCache = hitOrMiss(details, address, Line, mainCache);
    if (!inCache)
        fillCache(mainCache, details, address, Line, rpCache);
    if (details.RP == 0)
        updateLRU(details, address, Line, rpCache);
    else
        updateFIFO(details, address, Line, rpCache);
}