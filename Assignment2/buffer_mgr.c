#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"

//Frame information structure
typedef struct Frame{
    char *data; //pointer to data
    PageNumber pageNum; //page number in the page file
    int frameNum; //frame number in the buffer pool
    int fixcount; //counts users reading frame
    bool dirty; //boolean for if the frame has been changed
    struct Frame *previousFrame
    struct Frame *nextFrame
    long timeStamp //time stamp
} Frame;

typedef struct BufferPool{
    Frame *bufferPool; //pointer to buffer pool
    Frame *firstFrame; //pointer to first frame
    Frame *lastFrame; //pointer to last frame
    int readNum; //number of pages read
    int writeNum; // number of pages written
} Bufferpool;

long globalTime=0;

//initialize the Buffer Pool
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStratetgy strategy,
                  void *stratData){
    //creates buffer pool with size of the number of pages in the frame
    Frame *BP = (Frame *) malloc(numPages * sizeof(Frame));
    
    int i=0;
    while (i < numPages){
        BP[i].data = (char *) malloc(PAGE_SIZE * sizeof(char)); //Allocate memory
        BP[i].frameNum = i; //Set frame number
        BP[i].pageNum = NO_PAGE; //set to no page since initialize
        BP[i].fixcount = 0; //initialize to 0 since no access
        BP[i].dirty = false; //initialize to false since no change
        BP[i].timeStamp = 0;
        
        //set next and previous pointers
        if (i == 0){
            BP[i].previousFrame = NULL;
        }
        else {
            BP[i].previousFrame = &BP[i-1];
        }
        if (i == numPages - 1){
            BP[i].nextFrame = NULL;
        }
        else{
            BP[i].nextFrame = &BP[i+1];
        }
        i++;
    }
    
    //Buffer Pool information
    BufferPool *buffP = (BufferPool *) malloc(sizeof(BufferPool));
    buffP->bufferPool = bufferPool;
    buffPool->firstFrame = &bufferPool[0];
    buffPool->lastFrame = &bufferPool[numPages-1];
    buffPool->readNum = 0;
    buffPool->writeNum = 0;
    
    //Buffer pool struct
    bm->pageFile = (char*) pageFileName;
    bm->numPages= numPages;
    bm->strategy = strategy;
    bm->mgmtData = buffP;
    globalTime = 0;
    return RC_OK;
}

//Shut down the Buffer Pool
RC shutdownBufferPool(BM_BufferPool *const bm){
    
    int numPages = bm->numPages; //get numebr of pages
    BufferPool *buffP = bm->mgmtData;
    Frame *bufferPool = buffP->bufferPool;
    
    //get fix counts and dirty variables
    int *fixcounts = getFixCounts(bm);
    bool *dirtyflags = getDirtyFlags(bm);
    
    //Make sure fix counts is 0, otherwise do not shutdown and return error
    int i = 0;
    while (i < numPages){
        if (*(fixcounts + i) != 0){
            free(fixcounts);
            free(dirtyflags);
            return RC_PINNED_PAGES;
        }
        i++;
    }
    
    //Check if any frames are dirty
    int i = 0;
    while (i < numPages){
        if (*(dirtyflags + i) == true){
            SM_FileHandle fileHandle;
            char *fileName = bm->pageFile;
            openPageFile(fileName, &fileHandle);
            writeBlock(bufferPool[i].pageNum, &fileHandle, bufferPool[i].data);
            buffP->writeNum++;
            closePageFile(&fileHandle);
        }
        i++;
    }
    
    //free up memory and return
    free(buffP);
    free(fixcounts);
    free(dirtyflags);
    return RC_OK;
}

//Force Flush
RC forceFlushPool(BM_bufferPool *const bm){
    
    int numPages = bm->numPages; //get number of pages
    //set up file handle and get buffer pool information
    char *file = bm->pageFile;
    SM_FileHandle fileHandle;
    SM_PageHandle pageHandle = (SM_PageHandle) malloc(sizeof(SM_PageHandle));
    
    BufferPool *buffP = bm->mgmtData;
    //get dirty and fix counts variables
    bool *dirtyflags = getDirtyFlags(bm);
    int *fixcounts = getFixCounts(bm);
    
    int i = 0;
    while (i < numPages){
        if ((*(dirtyflags + i) == true) && (*(fixcounts + i) == 0)){
            //load frame and page number
            Frame *f = &(buffP->bufferPool[i]);
            PageNumber pageNum = f->pageNum;
            //write to file
            strcpy(pageHandle, f->data);
            openPageFile(fileName, &fileHandle);
            ensureCapacity(f->pageNum+1,&fileHandle);
            writeBlock(pageNum, &fileHandle, pageHandle);
            closePageFile(&fileHandle);
            frame-dirty = false;
            buffP->writeNum++;
        }
        i++;
    }
    
    free(fixcounts);
    free(dirtyflags)
    return RC_OK;
}

RC FIFO(BM_BufferPool *const bm, BM_PageHandle *const page, PageNumber pageNum){
    return RC_OK;
}

RC LRU(BM_BufferPool *const bm, BM_PageHandle *const page, PageNumber pageNum){
    return RC_OK;
}
