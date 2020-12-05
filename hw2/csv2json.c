#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#define  DATA_SIZE 7000000
#define  BUF_SIZE 420

int part;
int	thread_max;

typedef struct{
	int 	num;
	char 	**buf;
}arg_t;

void *csv2json(void *arg);

int main(int argc, char *argv[]){
	FILE 	*fin;
	FILE 	*fout;
	int 	num = 0;
	int		i;
	time_t	start;
	time_t	end;
	double	diff;
	char 	**data;
	char	tmp[300];
	
	start = time(NULL);
	
	fin = fopen("input.csv", "r");
	fout = fopen("output.json", "w+");

	data = (char**)malloc(sizeof(char*) * DATA_SIZE);
	
	while (fgets(tmp, BUF_SIZE, fin) != NULL) {
		data[num] = (char*)malloc(sizeof(char) * BUF_SIZE);
		strcpy(data[num++], tmp);
	}
	
	thread_max = atoi(argv[1]);
	pthread_t threads[thread_max];
	arg_t *arg = (arg_t*)malloc(sizeof(arg_t));
	arg -> num = num;
	arg -> buf = data;

	for (i = 0; i < thread_max; i++) {
		pthread_create(&threads[i], NULL, csv2json, (void*)arg);
	}
	for (i = 0; i < thread_max; i++) {
		pthread_join(threads[i], NULL);
	}

	fprintf(fout, "[\n");
	for (i = 0; i < num; i++) {
		fprintf(fout, "%s", data[i]);
		if (i < num - 1) {
			fprintf(fout, ",\n");
		}
	}
	fprintf(fout, "\n]");
	
	fclose(fin);
	fclose(fout);
	
	end = time(NULL);
	diff = difftime(end, start);;
	
	printf("%lf\n", diff);
	
	return 0;
}

void *csv2json(void *arg){
	int 	tmp[20];
	int 	i;
	int		left;
	int 	right;
	arg_t	*data = (arg_t*)arg;

	left = (data -> num) * part / thread_max;
	right =  (data -> num) * ++part / thread_max;
	
	for (i = left; i < right; i++) {
		sscanf(data -> buf[i], "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",
			   &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5], &tmp[6],
			   &tmp[7], &tmp[8], &tmp[9], &tmp[10], &tmp[11], &tmp[12], &tmp[13],
			   &tmp[14], &tmp[15], &tmp[16], &tmp[17], &tmp[18], &tmp[19]);
		sprintf(data -> buf[i], "{\n\"col_1\":%d,\n\"col_2\":%d,\n\"col_3\":%d,\n\"col_4\":%d,\n\"col_5\":%d,\n\"col_6\":%d,\n\"col_7\":%d,\n\"col_8\":%d,\n\"col_9\":%d,\n\"col_10\":%d,\n\"col_11\":%d,\n\"col_12\":%d,\n\"col_13\":%d,\n\"col_14\":%d,\n\"col_15\":%d,\n\"col_16\":%d,\n\"col_17\":%d,\n\"col_18\":%d,\n\"col_19\":%d,\n\"col_20\":%d\n}",
				tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7], tmp[8], tmp[9], tmp[10], tmp[11], tmp[12], tmp[13], tmp[14], tmp[15], tmp[16], tmp[17], tmp[18], tmp[19]);
	}
	
	pthread_exit(NULL);
}
