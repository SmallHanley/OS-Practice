#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <utility>
#include <queue>
using namespace std;
#define PAGE_SIZE 100000000
typedef pair<int, int> P;

struct compare{  
    bool operator()(const P& l, const P& r){  
        return l.second > r.second;  
    }  
 }; 

int main(int argc, char *argv[]){
    int *arr = new int[PAGE_SIZE];
    FILE *fin;
    int fileNum = 0, count = 0, tmp;
    clock_t start, end, t1;
    double cpu_time_used;
    start = clock();
    fin = fopen(argv[1], "r");
    while(fscanf(fin, "%d", &tmp) != EOF){
        if(count < PAGE_SIZE){
            arr[count++] = tmp;
        }
        if(count == PAGE_SIZE){
            FILE *fout;
            ++fileNum;
            char *file = new char[16];
            sprintf(file, "%d", fileNum);
            strcat(file, ".txt");
            fout = fopen(file, "w + t");
            sort(arr, arr + count);
            for(int i = 0; i < count; i++){
                fprintf(fout, "%d\n", arr[i]);
            }
            count = 0;
            fclose(fout);
            delete(file);
        }
    }
    if(count){
        FILE *fout;
        ++fileNum;
        char *file = new char[16];
        sprintf(file,"%d",fileNum);
        strcat(file, ".txt");
        fout = fopen(file, "w + t");
        sort(arr, arr + count);
        for(int i = 0; i < count; i++){
            fprintf(fout, "%d\n", arr[i]);
        }
        count = 0;
        fclose(fout);
        delete(file);
        fclose(fin);
    }
    delete(arr);
    t1 = clock();
    cpu_time_used = ((double) (t1 - start)) / CLOCKS_PER_SEC;
    printf("Read and sort time: %f\n", cpu_time_used);

    FILE *finput[fileNum], *foutput;
    for(int i = 0; i < fileNum; i++){
        char *file = new char[16];
        sprintf(file, "%d", i + 1);
        strcat(file, ".txt");
        finput[i] = fopen(file, "r");
        delete(file);
    }
    foutput = fopen("output.txt", "w + t");
    priority_queue<P, vector<P>, compare> pq;
    for(int i = 0; i < fileNum; i++ ){
        fscanf(finput[i], "%d", &tmp);
        pq.push(P(i, tmp));
    }
    while(!pq.empty()){
        P tmpP = pq.top();
        pq.pop();
        fprintf(foutput, "%d\n", tmpP.second);
        if(fscanf(finput[tmpP.first], "%d", &tmp) != EOF){
            pq.push(P(tmpP.first, tmp));
        }
    }
    for(int i = 0; i < fileNum; i++){
        char *file = new char[16];
        sprintf(file, "%d", i + 1);
        strcat(file, ".txt");
        fclose(finput[i]);
        remove(file);
        delete(file);
    }
    fclose(foutput);
    end = clock();
    cpu_time_used = ((double) (end - t1)) / CLOCKS_PER_SEC;
    printf("Merge time: %f\n", cpu_time_used);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Total time: %f\n", cpu_time_used);
    return 0;
}
