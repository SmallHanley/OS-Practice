# 2020_OS_Fall_HW1: Benchmark Your Computer Black Box
***繳交期限：09/21-10/06 00:00***

### 作業目標
* 請撰寫一支數值排序的程式，其功能是能夠將數值由小排序到大，且排序的資料量會遠大於電腦記憶體的容量。
* 請觀察及分析程式執行期間，包括但不限於CPU、Memory、Disk I/O的使用情況，探討作業系統是如何服務我們的程式。

---
**學號：** F74071069
**姓名：** 林霆寬
**系級：** 資訊111
#### 開發環境：
* OS：Ubuntu 20.04 虛擬機
* CPU：Intel® Core™ i5-8250U CPU @ 1.60GHz
* Memory：4GB
* Programming Language：C++

#### 程式執行時間：
$5\times10^8$ 筆測資 (約為5.11GB的txt)
![](https://i.imgur.com/4yaWTvL.png)
(單位：秒)

#### 程式開發與使用說明：
首先我先將input.txt分批讀進來，一個批次資料量足夠小使得記憶體能夠進行內部排序，我使用C++ algorithm的sort協助完成內部排序，排序好後的資料再依次依編號存入暫存的txt檔。大檔案拆成各自小檔案排序好後，我使用C++的priority_queue，用min heap的方式協助我完成之後的merge。先將每個檔案的第一個數加進min heap後，跑一個while迴圈。迴圈中，從min heap中取出最小的數字，放進output.txt中。若取出的數字其來源檔案還有未處理數字，則將該數字加進min heap中。若min heap為空，則跳出迴圈，完成merge。

```bash
# Compile
$ g++ -o brenchmark brenchmark.cpp
# Run
$ ./brenchmark [Data Path]
```

#### 效能分析報告：
首先，我先試著改變虛擬機的核心數，觀察硬體核心數的不同是否會影響程式的執行速度，進而推測出作業系統可能的行為。(上面所說的改變虛擬機的核心數量可能會跟真實情況有出入)
以下三張圖片是分別在1核心、2核心及4核心下觀察到的結果，這裡用的是single thread的程式：
* 1核心 
![](https://i.imgur.com/4yaWTvL.png)
* 2核心
![](https://i.imgur.com/9M7J7xU.png)
* 4核心
![](https://i.imgur.com/T0iMDhU.png)
<br>

從以上三張圖可以觀察到，single thread的程式在1核心的表現明顯比在2核心及4核心的表現還要好。再用一張圖片進一步觀察4核心CPU的使用情況，下圖擷取至system monitor：

![](https://i.imgur.com/lQJ9J0H.png)

single thread程式在多核心的環境下，很容易可以觀察到CPU使用率會有此消彼漲的情況。如圖所示，黃色、紅色、綠色線先後達到使用率約100%，推測作業系統因為排程的關係可能會將程式切換到不同處理器上，一些data比方說register file的資料也會藉由external的data path轉移至其他處理器上，我推測這是single thread在多核心環境下跑得比單核心還慢的原因。

於是乎，我用單核心的環境測試此single thread程式，觀察CPU及記憶體使用率的變化，來試圖找出優化方式以及推測作業系統背後可能提供的服務。

![](https://i.imgur.com/cfunBBj.png)

這是我收集資料的指令，先用top指令觀察執行中各個時期的CPU、記憶體使用率，將它匯出成txt檔案，再整理成圖表以觀察變化。

![](https://i.imgur.com/nWxpSgk.png)

我的input.txt為 $5\times10^8$ 筆測資，而我使用的k ways external merge sort的page size是$1\times10^8$ 筆測資，所以我的程式會把 $5\times10^8$ 筆測資的大檔案拆分成 $1\times10^8$ 筆測資sort好的小檔案。從圖中的前半部可以很容易地觀察出有約略5個週期的變化。其中，高的使用率是資料再做sort，而低的使用率是程式在讀寫檔案。後半部CPU的使用率則大致上都是在80%以下。由此可見sort的計算需求是相對較大的，於是程式對於硬體的優化我試著從此下手。

我採用multithread的程式設計來優化，降低CPU的閒置資源，增加計算速度。這邊使用data parallelism的方式，使用C language的pthread，將一段記憶體拆成n段，分給n個thread進行排序。

下表為 $1\times10^7$ 筆測資，n個thread，只收集純sort的秒數：



| n (thread) | 1 | 2 | 4 | 6 | 8 |
| -------- | -------- | -------- | -------- | -------- | -------- |
| 時間 (秒)     | 2.132     | 600以上     | 210     | 0.717     | 0.294     |

平均下來，6 threads和8 threads的時間比singel thread來得好，而2 threads和4 threads很突兀的時間遠高於single thread的好幾倍，我推測是資料量不夠大，用少量的threads中間link的成本太高，以及寫法的關係，pthread_join需要時常等待全部threads做完。除此之外，8 threads標準差過大，有時會測到40、50秒的秒數，整體下來，data parallelism沒有很明顯的優化。

![](https://i.imgur.com/y9HOazF.png)

至於記憶體的使用率，整個執行時間下來都在10%以下，前半部因為陣列以及sort遞迴，使用率比較高，而後半部只用了min heap來merge，使用率很低。但整體下來，記憶體的使用率不高，作業系統還有保留許多閒置資源。
![](https://i.imgur.com/hino9io.png)

**同時測試多支程式：**
這裡以2支程式同時執行來觀察：

![](https://i.imgur.com/KlYGYPt.png)

![](https://i.imgur.com/7pUhUjX.png)

![](https://i.imgur.com/ulfh9x2.png)

可以觀察到，同時執行兩個程式，閒置資源一樣多，但是時間慢了許多，兩支程式要搶CPU及HDD IO 資源。

**結論OS的設計要提供哪些優化服務：**
我認為作業系統CPU方面排程可以讓CPU使用率大的程式優先順位高一點，盡量在該程式CPU使用率低的時候，比方說memory IO，空出CPU資源，當CPU使用率高的時候，減少CPU的切換，才能增加效率。而記憶體方面，當有閒置資源時，可以減少閒置資源，增加記憶體使用率，減少虛擬記憶體。
