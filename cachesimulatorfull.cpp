/*
 Cache Simulator
 Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
 The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
 s = log2(#sets)   b = log2(block size)  t=32-s-b
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
    int L1blocksize;
    int L1setsize;
    int L1size;
    int L2blocksize;
    int L2setsize;
    int L2size;
};

/* you can define the cache class here, or design your own data structure for L1 and L2 cache
 class cache {
 
 }
 */

struct row{
    int tag;
    int valid;
    int dirty;
};

class CacheFull
{
public:
    // initialization, index # is cache size * 1024 / Associativity / Blocksize
    CacheFull(int blocksize, int setsize, int size)
    {
        way = setsize;
        tagnumber = size * 1024 / blocksize;
        cache.resize(tagnumber);
        for (int i = 0; i < tagnumber; i++)
        {
            cache[i].resize(1);
        }
    }
    // check if cache has corresponding tag
    bool hasTag(int index, int T)
    {
        for (int i = 0; i < tagnumber; i++)
        {
            if (cache[i][1].tag == T)
            {
                whichtag = i;
                return true;
            }
        }
        return false;
    }
    // check if valid bit is 1
    bool isValid(int index)
    {
        if (cache[whichtag][1].valid == 1) return true;
        return false;
    }
    // if the block to be evicted is dirty
    bool isDirty(int index)
    {
        int toEvict = counter % tagnumber;
        counter = toEvict;
        if (cache[toEvict][1].dirty == 1) return true;
        return false;
    }
    // check if cache has empty space
    bool hasEmpty(int index)
    {
        for (int i = 0; i < tagnumber; i++)
        {
            if (cache[i][1].valid == 0)
            {
                emptytag = i;
                return true;
            }
        }
        return false;
    }
    // write data to the block not dirty
    void writeOverlap(int index, int T)
    {
        int toEvict = counter % way;
        counter = toEvict;
        cache[toEvict][1].tag = T;
        cache[toEvict][1].valid = 1;
        cache[toEvict][1].dirty = 1;
        counter = counter + 1;
    }
    // write data to an empty space
    void writeEmpty(int index, int T)
    {
        cache[emptytag][1].tag = T;
        cache[emptytag][1].valid = 1;
        cache[emptytag][1].dirty = 1;
    }
    // write when tag match
    void writeTagMatch(int index, int T)
    {
        cache[whichtag][1].dirty = 1;
        cache[whichtag][1].valid = 1;
    }
    
    // return the address the evicted data has and replace the block with new data
    bitset<32> replaceCache(int index, int T, int Offset)
    {
        int toEvict = counter % way;
        counter = toEvict;

        bitset<32> toReturn;
        toReturn.reset();
        toReturn = toReturn.to_ulong() + Offset;
        int plusTag = cache[toEvict][1].tag * pow(2, log2(Offset));
        toReturn = toReturn.to_ulong() + plusTag;

        cache[toEvict][1].tag = T;
        cache[toEvict][1].valid = 1;
        cache[toEvict][1].dirty = 1;
        counter = counter + 1;

        return toReturn;
    }
    
private:
    int tagnumber;
    int way;
    int whichtag;
    int emptytag;
    vector<vector<row> > cache;
    int counter;
};

class Cache
{
public:
    // initialization, index # is cache size * 1024 / Associativity / Blocksize
    Cache(int blocksize, int setsize, int size)
    {
        way = setsize;
        indexnumber = size * 1024 / setsize / blocksize;
        cache.resize(indexnumber);
        counter.resize(setsize);
        for (int i = 0; i < indexnumber; i++)
        {
            cache[i].resize(setsize);
        }
    }
    
    // check if cache has corresponding tag
    bool hasTag(int index, int T)
    {
        for (int i = 0; i < way; i++)
        {
            if (cache[index][i].tag == T)
            {
                whichway = i;
                return true;
            }
        }
        return false;
    }
    // check if valid bit is 1
    bool isValid(int index)
    {
        if (cache[index][whichway].valid == 1) return true;
        return false;
    }
    // if the block to be evicted is dirty
    bool isDirty(int index)
    {
        int toEvict = counter[index] % way;
        counter[index] = toEvict;
        if (cache[index][toEvict].dirty == 1) return true;
        return false;
    }
    // check if cache has empty space
    bool hasEmpty(int index)
    {
        for (int i = 0; i < way; i++)
        {
            if (cache[index][i].valid == 0)
            {
                emptyway = i;
                return true;
            }
        }
        return false;
    }
    // write data to the block not dirty
    void writeOverlap(int index, int T)
    {
        int toEvict = counter[index] % way;
        counter[index] = toEvict;
        cache[index][toEvict].tag = T;
        cache[index][toEvict].valid = 1;
        cache[index][toEvict].dirty = 1;
        counter[index] = counter[index] + 1;
    }
    // write data to an empty space
    void writeEmpty(int index, int T)
    {
        cache[index][emptyway].tag = T;
        cache[index][emptyway].valid = 1;
        cache[index][emptyway].dirty = 1;
    }
    // write when tag match
    void writeTagMatch(int index, int T)
    {
        cache[index][whichway].dirty = 1;
        cache[index][whichway].valid = 1;
    }
    
    // return the address the evicted data has and replace the block with new data
    bitset<32> replaceCache(int index, int T, int Offset)
    {
        int toEvict = counter[index] % way;
        counter[index] = toEvict;

        bitset<32> toReturn;
        toReturn.reset();
        toReturn = toReturn.to_ulong() + Offset;
        int plusIndex = index * pow(2, log2(Offset));
        toReturn = toReturn.to_ulong() + plusIndex;
        int plusTag = cache[index][toEvict].tag * pow(2, log2(index) + log2(Offset));
        toReturn = toReturn.to_ulong() + plusTag;

        cache[index][toEvict].tag = T;
        cache[index][toEvict].valid = 1;
        cache[index][toEvict].dirty = 1;
        counter[index] = counter[index] + 1;

        return toReturn;
    }
    
private:
    int indexnumber;
    int way;
    int whichway;
    int emptyway;
    vector<vector<row> > cache;
    vector<int> counter;
};

int main(int argc, char* argv[]){
    
    
    
    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
        cache_params>>dummyLine;
        cache_params>>cacheconfig.L1blocksize;
        cache_params>>cacheconfig.L1setsize;
        cache_params>>cacheconfig.L1size;
        cache_params>>dummyLine;
        cache_params>>cacheconfig.L2blocksize;
        cache_params>>cacheconfig.L2setsize;
        cache_params>>cacheconfig.L2size;
    }
    
    
    int offset1bit;
    int index1bit;
    int tag1bit;
    // get cache1 and cache offset bits, index bits and tag bits
    if (cacheconfig.L1setsize == 0)
    {
        offset1bit = log2(cacheconfig.L1blocksize);
        index1bit = 0;
        tag1bit = 32 - offset1bit;
    }
    else
    {
        offset1bit = log2(cacheconfig.L1blocksize);
        index1bit = log2(cacheconfig.L1size * 1024 / cacheconfig.L1setsize / cacheconfig.L1blocksize);
        tag1bit = 32 - offset1bit - index1bit;
    }
    
    int offset2bit;
    int index2bit;
    int tag2bit;
    if (cacheconfig.L2setsize == 0)
    {
        offset2bit = log2(cacheconfig.L2blocksize);
        index2bit = 0;
        tag2bit = 32 - offset2bit;
    }
    else
    {
        offset2bit = log2(cacheconfig.L2blocksize);
        index2bit = log2(cacheconfig.L2size * 1024 / cacheconfig.L2setsize / cacheconfig.L2blocksize);
        tag2bit = 32 - offset2bit - index2bit;
    }

    
    
    
    // Implement by you:
    // initialize the hirearch cache system with those configs
    // probably you may define a Cache class for L1 and L2, or any data structure you like
    

    if (cacheconfig.L2setsize == 0)
    {
        CacheFull cache1(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size);
    }
    else
    {
        Cache cache1(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size);
    }
    
    if (cacheconfig.L2setsize == 0)
    {
        CacheFull cache2(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);
    }
    else
    {
        Cache cache2(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);
    }
    
    
    int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
    
    
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    
    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
    
    if (traces.is_open()&&tracesout.is_open()){
        while (getline (traces,line)){   // read mem access file and access Cache
            
            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
            
            //cout << accessaddr.to_string() << endl;
            //cout << accessaddr.to_ulong() << endl;
            
            // get the tag, index, offset of the address for L1
            unsigned long int tag1 = accessaddr.to_ulong() / pow(2, (offset1bit + index1bit));
            // when setsize is 0, this one should be 0
            unsigned long int index1 = (accessaddr.to_ulong() % int(pow(2, (offset1bit + index1bit)))) / pow(2, (offset1bit));
            unsigned long int offset1 = accessaddr.to_ulong() % int(pow(2, (offset1bit)));
            
            // get the tag, index, offset of the address for L2
            unsigned long int tag2 = accessaddr.to_ulong() / pow(2, (offset2bit + index2bit));
            // when setsize is 0, this one should be 0
            unsigned long int index2 = (accessaddr.to_ulong() % int(pow(2, (offset2bit + index2bit)))) / pow(2, (offset2bit));
            unsigned long int offset2 = accessaddr.to_ulong() % int(pow(2, (offset2bit)));
            
            //cout << tag1 << endl;
            //cout << tag2 << endl;
            
            
            // access the L1 and L2 Cache according to the trace;
            if (accesstype.compare("R")==0)
                
            {
                //Implement by you:
                // read access to the L1 Cache,
                //  and then L2 (if required),
                //  update the L1 and L2 access state variable;
                
                // cache1: tag1 match hit
                if (cache1.hasTag(index1, tag1))
                {
                    // tag1 match, valid
                    if (cache1.isValid(index1))
                    {
                        cout << "1 0" << endl;
                    }
                    // tag1 match, invalid
                    else
                    {
                        // read L2
                        // tag2 match
                        if (cache2.hasTag(index2, tag2))
                        {
                            // tag2 match, valid, L2 read hit, write L1
                            if (cache2.isValid(index2))
                            {
                                cache1.writeTagMatch(index1, tag1);
                                cout << "2 1" << endl;
                            }
                            // tag2 match, invalid, L2 read miss, write L2, write L1
                            else
                            {
                                cache2.writeTagMatch(index2, tag2);
                                cache1.writeTagMatch(index1, tag1);
                                cout << "2 2" << endl;
                            }
                        }
                        // tag2 miss
                        else
                        {
                            // read memory write L2, write L1
                            // cache2 has empty space, write L2, write L1
                            if (cache2.hasEmpty(index2)) 
                            {
                                cache2.writeEmpty(index2, tag2);
                                cache1.writeTagMatch(index1, tag1);
                                cout << "2 2" << endl;
                            }
                            // cache2 has no empty space, replace L2, write L1
                            else
                            {
                                cache2.replaceCache(index2, tag2, offset2);
                                cache1.writeTagMatch(index1, tag1);
                                cout << "2 2" << endl;
                            }
                        }
                    }
                }
                // cache1: tag1 miss
                else
                {
                    // read L2
                    // L2 tag2 match
                    if (cache2.hasTag(index2, tag2))
                    {
                        // L2 tag2 match, valid, Read L2 hit, write L1
                        if (cache2.isValid(index2))
                        {
                            //cout << "L2 read b77aef3c" << endl;
                            // L1 has empty space
                            if (cache1.hasEmpty(index1))
                            {
                                //cout << "L1 write b77aef3c" << endl;
                                cache1.writeEmpty(index1, tag1);
                                cout << "2 1" << endl;
                            }
                            // L1 has no empty space
                            else
                            {
                                // if the data block to be evicted is dirty
                                if (cache1.isDirty(index1))
                                {
                                    bitset<32> toCache2 = cache1.replaceCache(index1, tag1, offset1);
                                    unsigned long int tag1to2 = toCache2.to_ulong() / 2^(offset2bit + index2bit);
                                    // when L2 setsize is 0, index1to2 is offset
                                    unsigned long int index1to2 = (toCache2.to_ulong() % 2^(offset2bit + index2bit)) % 2^(offset2bit);
                                    // L2 write hit, update L2
                                    if (cache2.hasTag(index1to2, tag1to2) && cache2.isValid(index1to2))
                                    {
                                        cache2.writeTagMatch(index1to2, tag1to2);
                                        cout << "2 1" << endl;
                                    }
                                    // L2 write miss, do nothing
                                    else cout << "2 1" << endl;
                                }
                                // if the data block to be replaced is not dirty
                                else
                                {
                                    cache1.writeOverlap(index1, tag1);
                                    cout << "2 1" << endl;
                                }
                            }
                        }
                        // L2 tag2 match, invalid, Read L2 miss, Read Mem
                        else
                        {
                            cache2.writeTagMatch(index2, tag2);
                            // write L1
                            // if L1 has empty space
                            if (cache1.hasEmpty(index1))
                            {
                                cache1.writeEmpty(index1, tag1);
                                cout << "2 2" << endl;
                            }
                            // L1 not empty
                            else
                            {
                                // if the data block to be replaced dirty
                                if (cache1.isDirty(index1))
                                {
                                    bitset<32> toCache2 = cache1.replaceCache(index1, tag1, offset1);
                                    unsigned long int tag1to2 = toCache2.to_ulong() / 2^(offset2bit + index2bit);
                                    //when L2 setsize is 0, index1to2 is offset
                                    unsigned long int index1to2 = (toCache2.to_ulong() % 2^(offset2bit + index2bit)) % 2^(offset2bit);
                                    // L2 write hit, update L2
                                    if (cache2.hasTag(index1to2, tag1to2) && cache2.isValid(index1to2))
                                    {
                                        cache2.writeTagMatch(index1to2, tag1to2);
                                        cout << "2 2" << endl;
                                    }
                                    // L2 write miss, do nothing
                                    else cout << "2 2" << endl;
                                }
                                // if the data block to be replaced not dirty
                                else
                                {
                                    cache1.writeOverlap(index1, tag1);
                                    cout << "2 2" << endl;
                                }
                            }
                        }
                    }
                    // tag2 miss, read memory, write L2
                    else
                    {
                        // if cache2 has empty space, L2 not evict
                        if (cache2.hasEmpty(index2))
                        {
                            //cout << "L2 write b77aef34" << endl;
                            cache2.writeEmpty(index2, tag2);
                            // write L1
                            // if L1 has empty space
                            if (cache1.hasEmpty(index1))
                            {
                                //cout << "L1 write b77aef34" << endl;
                                cache1.writeEmpty(index1, tag1);
                                cout << "2 2" << endl;
                            }
                            // if L1 has no empty space
                            else
                            {
                                // if the data block to be replaced in L1 is dirty
                                if (cache1.isDirty(index1))
                                {
                                    bitset<32> toCache2 = cache1.replaceCache(index1, tag1, offset1);
                                    unsigned long int tag1to2 = toCache2.to_ulong() / 2^(offset2bit + index2bit);
                                    //when L2 setsize is 0, index1to2 is offset
                                    unsigned long int index1to2 = (toCache2.to_ulong() % 2^(offset2bit + index2bit)) % 2^(offset2bit);
                                    // L2 write hit, update L2
                                    if (cache2.hasTag(index1to2, tag1to2) && cache2.isValid(index1to2))
                                    {
                                        cache2.writeTagMatch(index1to2, tag1to2);
                                        cout << "2 2" << endl;
                                    }
                                    // L2 write miss, do nothing
                                    else cout << "2 2" << endl;
                                }
                                // if the data block to be replaced in L1 is not dirty
                                else
                                {
                                    cache1.writeOverlap(index1, tag1);
                                    cout << "2 2" << endl;
                                }
                            }
                        }
                        // if cache2 has no empty space
                        else
                        {
                            // if data block to be replaced in L2 is dirty
                            if (cache2.isDirty(index2))
                            {
                                cache2.replaceCache(index2, tag2, offset2);
                                // write L1
                                // if L1 has empty space
                                if (cache1.hasEmpty(index1))
                                {
                                    cache1.writeEmpty(index1, tag1);
                                    cout << "2 2" << endl;
                                }
                                // if L1 has no empty space
                                else
                                {
                                    // if data block to be replaced in L1 is dirty
                                    if (cache1.isDirty(index1))
                                    {
                                        bitset<32> toCache2 = cache1.replaceCache(index1, tag1, offset1);
                                        unsigned long int tag1to2 = toCache2.to_ulong() / 2^(offset2bit + index2bit);
                                        //when L2 setsize is 0, index1to2 is offset
                                        unsigned long int index1to2 = (toCache2.to_ulong() % 2^(offset2bit + index2bit)) % 2^(offset2bit);
                                        // L2 write hit, update L2
                                        if (cache2.hasTag(index1to2, tag1to2) && cache2.isValid(index1to2))
                                        {
                                            cache2.writeTagMatch(index1to2, tag1to2);
                                            cout << "2 2" << endl;
                                        }
                                        // L2 write miss, do nothing
                                        else cout << "2 2" << endl;
                                    }
                                    // if data block to be replaced in L1 is not dirty
                                    else
                                    {
                                        cache1.writeOverlap(index1, tag1);
                                        cout << "2 2" << endl;
                                    }
                                }
                            }
                            // if data block to be replaced in L2 is not dirty
                            else
                            {
                                cache2.writeOverlap(index2, tag1);
                                // write L1
                                // if L1 has empty space
                                if (cache1.hasEmpty(index1))
                                {
                                    cache1.writeEmpty(index1, tag1);
                                    cout << "2 2" << endl;
                                }
                                // if L1 has no empty space
                                else
                                {
                                    // if data block to be replaced in L1 is dirty
                                    if (cache1.isDirty(index1))
                                    {
                                        bitset<32> toCache2 = cache1.replaceCache(index1, tag1, offset1);
                                        unsigned long int tag1to2 = toCache2.to_ulong() / 2^(offset2bit + index2bit);
                                        //when L2 setsize is 0, index1to2 is offset
                                        unsigned long int index1to2 = (toCache2.to_ulong() % 2^(offset2bit + index2bit)) % 2^(offset2bit);
                                        // L2 write hit, update L2
                                        if (cache2.hasTag(index1to2, tag1to2) && cache2.isValid(index1to2))
                                        {
                                            cache2.writeTagMatch(index1to2, tag1to2);
                                            cout << "2 2" << endl;
                                        }
                                        // L2 write miss, do nothing
                                        else cout << "2 2" << endl;
                                    }
                                    // if data block to be replaced in L1 is not dirty
                                    else
                                    {
                                        cache1.writeOverlap(index1, tag1);
                                        cout << "2 2" << endl;
                                    }
                                }
                            }
                        }
                    }
                }
                
                
                
            }
            else
            {
                //Implement by you:
                // write access to the L1 Cache,
                //and then L2 (if required),
                //update the L1 and L2 access state variable;
                
                // cache1 write
                // tag1 match
                if (cache1.hasTag(index1, tag1))
                {
                    // L1 write hit
                    if (cache1.isValid(index1))
                    {
                        cache1.writeTagMatch(index1, tag1);
                        cout << "3 0" << endl;
                    }
                    // L1 write miss: tag match, invalid
                    else
                    {
                        // L2 tag match
                        if (cache2.hasTag(index2, tag2))
                        {
                            // L1 miss L2 hit:
                            if (cache2.isValid(index2))
                            {
                                cache2.writeTagMatch(index2, tag2);
                                cout << "4 3" << endl;
                            }
                            // L1 miss L2 miss: tag2 match, invalid, do nothing
                            else
                            {
                                cout << "4 4" << endl;
                            }
                        }
                        // L1 miss L2 miss: tag2 unmatch do nothing
                        else
                        {
                            cout << "4 4" << endl;
                        }
                    }
                }
                // tag1 miss, write L2
                else
                {
                    // L2 tag match
                    if (cache2.hasTag(index2, tag2))
                    {
                        // L1 miss L2 hit:
                        if (cache2.isValid(index2))
                        {
                            cache2.writeTagMatch(index2, tag2);
                            cout << "4 3" << endl;
                        }
                        // L1 miss L2 miss: tag2 match, invalid, do nothing
                        else
                        {
                            cout << "4 4" << endl;
                        }
                    }
                    // L1 miss L2 miss: tag2 unmatch do nothing
                    else
                    {
                        cout << "4 4" << endl;
                    }
                }
                
                
                
                
                
            }
            
            
            
            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
            
            
        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";
    
    
    
    
    
    
    
    return 0;
}
