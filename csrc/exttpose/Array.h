#ifndef __ARRAY_H
#define __ARRAY_H
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>

using namespace std;

class Array {
protected:
   int *theArray;
   char theFlg;
   int lastPos;
   unsigned int theSize;
   unsigned int totSize;
   long *offset;
public:
   
   Array(int sz, int npart=1);
   ~Array();

   int operator [] (unsigned int index)
   {
      return theArray[index];
   };

   char flg()
   {
      return theFlg;
   }
   void setflg(char flg)
   {
      theFlg = flg;
   }
   int lastpos()
   {
      return lastPos;
   }

   //to be used ony for use_seq
   void setlastpos()
   {
      theArray[lastPos+1] = theSize-lastPos-2;
      lastPos = theSize;
   }
   long get_offset(int pos=0)
   {
      return offset[pos];
   }
   void set_offset(long off, int pos=0)
   {
      offset[pos] = off;
   }
   
   int totsize()
   {
      return totSize;
   }
   void reset()
   {
      theSize = 0;
      lastPos = 0;
      theFlg = 0;
   }

   int *array()
   {
      return theArray;
   }
   
   int size() 
   {
      return theSize; 
   }
   void setsize(int size)
   {
      theSize = size;
   }
   
   void setitem(int pos, int item)
   {
      theArray[pos] = item;
   }

   void additem(int item){
      theArray[theSize] = item;
      theSize++;      
   }
   
   void flushbuf(int fd, int use_seq, int pos=0)
   {
      lseek(fd, offset[pos]*sizeof(int),SEEK_SET);
//       int wblk = (use_seq==1) ? lastPos : theSize;
//       //if (lastPos != theSize)
//       //   cout << "WBLK " << wblk << " " << lastPos << " "
//       //        << theSize << endl << flush;
      int wblk = theSize;
      if (wblk > 0){
         int res = ::write(fd, (char *)theArray, wblk*sizeof(int));
         if (res < wblk*sizeof(int)){
            throw runtime_error("Error writing");
         }
         offset[pos] += wblk;
      }
      theSize = 0;
   }
   void add (int fd, int item, int use_seq, int pos, int custid=-1)
   {
      if (use_seq){
//          if (theSize+1+((custid == -1)?0:2)> totSize){
//             //cout << "WRITE " << item << " " << custid << " "
//             //     << offset << " " << lastPos << " " << theSize << " "
//             //     << totSize << endl << flush;
//             if (lastPos == 0 && custid == -1){
//                cout << "REALLOC " << totSize << " "<< theSize << endl;
//                totSize *= 2;
//                theArray = (int *)realloc(theArray, totSize*sizeof(int));
//                if (theArray == NULL){
//                   throw runtime_error("ERROR IN REALLOC Array::add");
//                   exit(errno);
//                }
//             }
//             else{
//                flushbuf(fd,use_seq,pos);
//                for (int i=0; lastPos < theSize; i++, lastPos++)
//                   theArray[i] = theArray[lastPos];
//                theSize = i;
//                lastPos = 0;
//                //cout << "WROTE " << theSize << " " << lastPos << " "<<
//                //   offset << endl <<flush;
//             }
//          }
//          if (custid !=-1){
//             theArray[theSize++] = custid; //store custid
//             theSize++; //for the tid count
//          }
         if (theSize+2 > totSize){
            flushbuf(fd,use_seq,pos);
         }
         theArray[theSize++] = custid;
      }
      else{
         if (theSize+1 > totSize){
            flushbuf(fd,use_seq,pos);
         }
      }
      theArray[theSize++] = item;
   }
//    void add (int fd, int item, int use_seq, int custid=-1)
//    {
//       if (theSize+1 > totSize){
//          totSize = (int) (totSize*2);
         
//          theArray = (int *)realloc(theArray, totSize*sizeof(int));
//          if (theArray == NULL){
//             cout << "MEMORY EXCEEDED\n";
//             exit(-1);
//          }
//       }
//       theArray[theSize] = item;
//       theSize++;
//    }
};
#endif //__ARRAY_H


