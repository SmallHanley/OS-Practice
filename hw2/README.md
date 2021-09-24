# 2020_OS_Fall_HW2: ETL process
:::info
**繳交期限：10/05-10/26 13:00**
:::
**作業目標**
- 請撰寫一支以多執行緒（Multi-Thread）開發的轉換程式，其功能是將CSV檔案轉換並輸出成JSON檔案。
- 請觀察及分析程式執行期間，包括但不限於CPU、Memory、Disk I/O的使用情況，探討作業系統是如何服務我們的程式。
___

**學號**：F74071069
**姓名**：林霆寬
**系級**：資訊111

### 開發環境
- OS: 虛擬機 Ubuntu 20.04
- CPU: Intel® Core™ i5-8250U CPU @ 1.6GHz × 6
- Ｍemory: 4GB
- Programming Language(version): C89 (ISO/IEC 9899)

### 程式執行時間
| 1        | 2        | 3        | 4        | 5        | 6        | 7        | 8        |
| -------- | -------- | -------- | -------- | -------- | -------- | -------- |:-------- |
| 90       | 83       | 78       | 78       | 82       | 77       | 75       | 76       |
| 97       | 97       | 86       | 78       | 84       | 78       | 76       | 77       |
| 101      | 84       | 76       | 76       | 82       | 86       | 76       | 69       |
| 100      | 83       | 78       | 72       | 81       | 77       | 66       | 79       |
| 99       | 85       | 80       | 73       | 74       | 75       | 79       | 77       |
| **97.4** | **84.2** | **78.0** | **76.6** | **79.4** | **78.6** | **74.4** | **75.6** |

▲ 表(一) 執行緒個數從 1 ~ 8 分別執行5次平均的時間，單位為秒

以上測試資料採用的大小約為 1.5 GB。這邊採用的是 real time，因為程式比較大，直接使用C語言的time()，精度到秒。

### 程式開發與使用說明
#### 程式開發流程
1. 先寫一支 single thread 程式，確保能夠正確執行並且結果正確。
2. Open 兩個 file 分別為 input.csv 和 output.json。然後宣告 char** 並 malloc 給他 DATA_SIZE 個 sizeof(char*) 的空間給它。接著用 fgets 一次從 input file 輸入一行直至檔案尾，同時每讀到一行就給 data[num] malloc BUF_SIZE 個 sizeof(char) 的空間，把讀進來的資料放進data[num]，num++。此方式可以避免一次 malloc 太大的連續空間會要不到。(輸入完後 num 即為 輸入的行數。)
3. 接著用 sscanf 從 data[num] parse 並暫存 20 個 int，然後用 sprintf 將暫存的 int 依照 json 格式放回 data[num]。完成 num 次後輸出到 output file，完成 single thread 程式。
4. 改成 multi-thread 程式，這裡真正執行 multi-thread 是第 3 點的部分。假設有 n 個 thread，則將 num 組數據分給 n 個 thread，每個 thread 的工作內容就是像第 3 點一樣透過 sscanf、sprintf 轉換 data，也就是用 multi-thread 進行 data parallelism。至於細部的分法請見以下說明。

**分法：**
假設 num 也就是行數為 50，thread 個數為 4。
這邊定義兩個整數 low、high，利用取下界不取上界來分，也就是 [low, high)

第 i 個 thread (i from 0 to num - 1)

low = $\lfloor50\ \times\ \dfrac{i}{4}\rfloor$
high = $\lfloor50\ \times\ \dfrac{i + 1}{4}\rfloor$

最後拆分結果如下：
no1: 0 ~ 12
no2: 12 ~ 25
no3: 25 ~ 37
no4: 37 ~ 50

#### 使用說明
```bash
# Compile
# gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
$ gcc -pthread -o csv2json csv2json.c
# Run
$ ./csv2json [threads]
```

### 效能分析報告

#### 執行時間觀察
根據 表(一)，可以畫出執行時間對不同執行時間的分布圖

![](https://i.imgur.com/6V3C5NJ.png)

▲ 圖(一) 不同執行緒5次平均執行時間分布，x軸為執行緒個數，y軸為平均執行時間(秒)

透過上圖，可以觀察到單執行緒的執行時間最久，隨著執行緒增加，執行時間慢慢降低，到了執行緒 4 個以上，變化幅度就沒那麼明顯，大約落在75 ~ 80 秒之間。

#### CPU使用率觀察
我先用以下指令記錄 top 中有關 CPU 和 MEM 的使用率到 record.txt。
```bash
$ top -b -d 0.5 -U <Username> > record.txt
```
並用 C++ 將想要的資料擷取下來並另存於 table.txt ，再用 Excel 整理成以下幾個圖表。
:::spoiler 詳細資訊 C++ 擷取
```c=
#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>
using namespace std;

int main(void){
    fstream fin;
    FILE *fout;
    fin.open("record.txt", ios::in);
    fout = fopen("table.txt", "w+");
    string s;
    char a[20], c[20], m[20], p[20];
    while (getline(fin, s)) {
    	size_t found = s.find("csv2json");
  		if (found != std::string::npos) {
		 	sscanf(s.c_str(), "%s %s %s %s %s %s %s %s %s %s %s %s", p, a, a, a, a, a, a, a, c, m, a, a);
			fprintf(fout, "%s %s %s\n", p, c, m);
		}   
    
	}
    fin.close();
    fclose(fout);
    return 0;
}
```
:::
<br>

以下資料為執行緒 1、2、4、6、8 分別執行一次的結果，測試資料均為 1.5 GB，圖表 x軸 時間單位以單執行緒執行時間作為 x軸 上界。

![](https://i.imgur.com/nLVsaPf.png)

▲ 圖(二) 不同執行緒隨時間的CPU使用率變化

本次分析虛擬機配置 6核 (6緒)，從上圖中可以觀察執行時間大致為 1緒 > 2緒 > 8緒 > 4緒 > 6緒，而 1緒 CPU 使用率約為 100%，2緒 CPU 使用率約為200%，4緒 CPU 使用率約為400%，6 及 8 緒 CPU 使用率約為600%。1、2 緒因為 CPU 使用率較低，在資料轉換的計算上比較費時，所以執行時間長。4、6 緒因為 CPU 使用率高，data parallelism 平行化大，圖中 CPU 使用率高起來的地方代表資料轉換的 stage，可以明顯觀察到黃線、綠線"ㄇ字型"的寬度比紅線、橘線小，代表轉換這個 stage 4、6 緒所花的時間比較少。至於 8緒，可以觀察到 CPU 使用率依然約為 600%，可以驗證程式的 thread 為虛擬 thread ，實際上實體 thread 還是要看作業系統如何分配。

#### MEM使用率觀察

![](https://i.imgur.com/Ng6gZfJ.png)

▲ 圖(三) 不同執行緒隨時間的MEM使用率變化

本次作業程式部分是採用測試資料多大，就 malloc 多大空間的設計，所以不同執行緒讀取的 stage 都是呈現慢慢增加，到了高點後，MEM 使用率不再增加，代表讀完檔案，可見不同執行緒讀檔階段用時差不多，因為真正creat thread 是在轉換階段。

#### CPU、MEM 疊圖觀察

![](https://i.imgur.com/Sbo1QGJ.png)

▲ 圖(四) 單執行緒隨時間的CPU、MEM使用率變化

從圖中可以很明顯觀察程式執行的三個時期
- 讀取時期
MEM 使用率慢慢增加到高點
- 轉換時期
當 MEM 使用率到高點時，CPU 使用率突然竄高並維持在高點
- 寫入時期
轉換完後，計算量降低，CPU使用率降低

#### 單獨看轉換時期
既然知道程式中只有轉換時期 creat thread，那就單獨分析轉換時期不同執行緒的執行時間。

| 1        | 2        | 3        | 4       | 5       | 6       | 7       | 8       |
| -------- | -------- | -------- | ------- | ------- | ------- | ------- |:------- |
| 32       | 16       | 11       | 9       | 9       | 8       | 8       | 9       |
| 32       | 16       | 12       | 9       | 14      | 8       | 8       | 9       |
| 32       | 16       | 11       | 9       | 8       | 8       | 8       | 8       |
| 32       | 16       | 12       | 9       | 8       | 9       | 8       | 9       |
| 32       | 17       | 11       | 9       | 8       | 8       | 8       | 8       |
| **32.0** | **16.2** | **11.4** | **9.0** | **9.4** | **8.2** | **8.0** | **8.6** |

▲ 表(二) 執行緒個數從 1 ~ 8 分別執行5次平均的轉換時間，單位為秒

從上表中可以觀察到不同執行緒在轉換時期的執行時間還是有落差，2緒時間約為 1緒的一半，4緒的時間也約為 2緒的一半，而 4緒以上轉換時間差不多。

#### 結論
以我的虛擬機 6核 (6緒) 做總結，當程式 creat 的虛擬 thread $\leq\ 6$ 時，作業系統分配實體 thread 的數量會約等於程式 creat 的虛擬 thread 的數量，從 圖(二) 可以看出來，開多少 threads，CPU 使用率大約就是 100% × thread 個數，所以執行時間會隨著 thread 個數增加而縮短。至於程式 creat 的虛擬 thread > 6 時，作業系統也只能分配最多 6緒 給該程式，所以執行時間不會因此減少。
