#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

// Include return codes and methods for logging errors
#include "dberror.h"

// Include bool DT
#include "dt.h"

#include "storage_mgr.h"

// Replacement Strategies
typedef enum ReplacementStrategy {
	RS_FIFO = 0,
	RS_LRU = 1,
	RS_CLOCK = 2,
	RS_LFU = 3,
	RS_LRU_K = 4
} ReplacementStrategy;

// Data Types and Structures
typedef int PageNumber;
#define NO_PAGE -1

typedef struct BM_PageHandle {
    PageNumber pageNum;
    char *data;
} BM_PageHandle;

typedef struct Frame{
    SM_PageHandle data; //pointer to data
    PageNumber pageNum; //page number in the page file
    int frameNum; //frame number in the buffer pool
    int fixCount; //counts users reading frame
    bool dirty; //boolean for if the frame has been changed
    struct Frame *previousFrame;
    struct Frame *nextFrame;
    long timeLastUsed; //time stamp for LRU
    long timeFirstPinned; //time stamp for FIFO
} Frame;

typedef struct BM_BufferPool {
	char *pageFile;
	int numPages;
	ReplacementStrategy strategy;
	void *mgmtData; // use this one to store the bookkeeping info your buffer
	// manager needs for a buffer pool
	Frame *firstFrame; //pointer to first frame
	Frame *lastFrame; //pointer to last frame
	int readNum; //number of pages read
	int writeNum; // number of pages written
} BM_BufferPool;




// convenience macros
#define MAKE_POOL()					\
		((BM_BufferPool *) malloc (sizeof(BM_BufferPool)))

#define MAKE_PAGE_HANDLE()				\
		((BM_PageHandle *) malloc (sizeof(BM_PageHandle)))

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		const int numPages, ReplacementStrategy strategy,
		void *stratData);
RC shutdownBufferPool(BM_BufferPool *const bm);
RC forceFlushPool(BM_BufferPool *const bm);

// Buffer Manager Interface Access Pages
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page);
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page);
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page);
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
		const PageNumber pageNum);
Frame* findPage(BM_BufferPool *const bm, BM_PageHandle *const page);
bool isBufferFull(BM_BufferPool *const bm);
Frame* findFreeFrame(BM_BufferPool *const bm);
Frame* checkExistingFrames(BM_BufferPool *const bm, const PageNumber pageNum);

// Statistics Interface
PageNumber *getFrameContents (BM_BufferPool *const bm);
bool *getDirtyFlags (BM_BufferPool *const bm);
int *getFixCounts (BM_BufferPool *const bm);
int getNumReadIO (BM_BufferPool *const bm);
int getNumWriteIO (BM_BufferPool *const bm);

// Replacement strategies
RC FIFO(BM_BufferPool *const bm, BM_PageHandle *const page, PageNumber pageNum);
RC LRU(BM_BufferPool *const bm, BM_PageHandle *const page, PageNumber pageNum);
#endif
