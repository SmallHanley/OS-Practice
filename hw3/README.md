# 2020_OS_Fall_HW3: Key-Value Stroages
###### tags: `Operating Systems 2020`
:::warning
繳交日期：11/16 ~ 12/14 13:00
:::

**作業目標**
* 請撰寫一支 Key-Value Stroages 的程式，其功能是能夠處理 PUT、GET、SCAN 指令，且處理的資料量必須大於電腦記憶體的容量。
* 請觀察及分析程式執行期間，包括但不限於CPU、Memory、Disk I/O的使用情況，探討作業系統是如何服務我們的程式。
* 請說明你所設計的整理資料作法，並分析這些設計在我們存取資料的時候，如何能提供較佳的存取效率。
___

**學號**：F74071069
**姓名**：林霆寬
**系級**：資訊111

## 開發環境
- OS: 虛擬機 Ubuntu 20.04
- CPU: Intel® Core™ i5-8250U CPU @ 1.6GHz × 6
- Ｍemory: 4GB
- Programming Language(version): C++11 (ISO/IEC 14882:2011)

## 程式執行時間
測資 test.input 為 PUT、GET 各十萬筆資料，其中 key 的分布為連續且集中。而測資 test2.input 為 PUT、GET 各十萬筆資料，key 的分布為亂序且分散。

![](https://i.imgur.com/ApLeB9Z.png)

![](https://i.imgur.com/HKBEcEa.png)

第一張圖為 hash table 使用 memory mapped I/O 的結果，第二張圖為使用 file I/O 的結果。

## 程式開發與使用說明
### 系統架構 (初步)
#### **Key - Value**

![](https://i.imgur.com/uvaSQVo.png)

將一個 key 拆成 tag 和 offset，tag 是 block 的編號，用來查詢 MemTable 以及 Hash Table，而 offset 代表是 block 的大小。

#### **In Memory**

![](https://i.imgur.com/s6NO7PS.png)

記憶體中實作一個 MemTable，類似作業系統中 paging 的概念。從前兩次作業可以得知 I/O 速度是比 memory 慢許多的，因此我採用 fully associative page table 的方式，降低 page fault (block miss) 發生的次數，而 table 滿的替換方式是採用 random 的 policy。細看一個 block 的組成，包含 tag、dirty 以及 128 個 valid 和 value 區域。Tag 表示這個 block 的編號，dirty 代表 block 是否需要寫回檔案中， valid 代表當行的 value 是否有效，而 value 則是存 128 bytes 的區域。

#### **In File**

![](https://i.imgur.com/y4ux9vS.png)

當 table 溢出或是程式結尾時，需要將 dirty 的 block 寫回檔案，當 key-value storage users 需要 value 時，需要將資料放到 memory 裡。要能迅速根據 key 查詢 value 所在位置，查詢複雜度不能太高。此架構採用的是 hash table 的方式，詳細實作架構如下。

#### **Hash Table**

![](https://i.imgur.com/3hTp4ZL.png)

此架構採用的是 disk-based hash table，因為 hash table 需要透過較大的空間來換取查詢的速度，並減少 conflict 的機率。透過 tag，經由 hash function 對應出來的值作為 hash table 的 index，在去對應位置查詢的值即為該 block 在檔案中的位置。當發生 conflict 時，採用的是找尋下一個位置，直到找到對應的 tag 所在的位置。若下一個位置為空，則代表 block 還未被存入檔案系統中，需要給它檔案中的位置，並更新 hash table。此架構的查詢複雜度 average case 為 O(1)，worst case 為 O(n)。

### 專案目錄
```bash
$ tree
.
├── hash_table.cpp
├── hash_table.h
├── kvs.cpp
├── Makefile
└── storage
```

### 使用說明
```bash
# Compile
# gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
$ make
g++ -g -Wall -std=c++11 -c kvs.cpp
g++ -g -Wall -std=c++11 -c hash_table.cpp
g++ -o kvs kvs.o hash_table.o 

# Run
$ ./kvs "input file"
```

## 分析報告

### 改變 block size (table size 固定)
* size = 128

![](https://i.imgur.com/P3Cr1Vu.png)

* size = 64

![](https://i.imgur.com/egoHlzg.png)

* size = 32

![](https://i.imgur.com/FQsrd6L.png)

* size = 16

![](https://i.imgur.com/fEcj8Tq.png)

* size = 8

![](https://i.imgur.com/pNYUyjf.png)

* size = 4

![](https://i.imgur.com/KmBrojQ.png)

觀察結果，若在 table size 固定的情況下，減少 block size，會增加 table 中放置 block 的數量。本來 block 設比較大 (7 bits) 是為了增加連續讀寫、SCAN、資料集中等的速度，但發現在資料很分散的時候，若 block 設太大，則會有很多空的空間需要在 memory 及 disk 之間被搬移，搬移的成本太高。除此之外，table 中能用的 block 相對比較少，在資料集中的情況下，或許 hit rate 會有不錯的表現，但是在資料很分散的情況下，則大大的增加了 block miss rate。於是目前的寫法是，犧牲連續讀寫的速度，減少 big block size 的 overhead。

### Memory Mapped I/O V.S. File I/O

![](https://i.imgur.com/ApLeB9Z.png)

![](https://i.imgur.com/HKBEcEa.png)

上圖為使用 mmap 做 hash table，下圖為使用 fread、fwrite 做 hash table。不管測試資料大小、分散集中狀況為何，mmap 的速度都會比 file I/O 慢一些，但是不會差異很多，猜測是我存取資料的方式是大範圍的 randomly access，使用 mmap 很容易造成 page fault，因此速度比較慢。我最終版本是採用 file I/O 的版本。
