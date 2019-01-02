/*
 * =====================================================================================
 *
 *       Filename:  tsp_sequential.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/30/2018 08:48:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#define MAX_DISTANCE 99999
#define BUFFERSIZE 4096
#define COPYMODE 0644

typedef struct{
    int** matrix;   // Store the distances between cities
    int* order;     // Store the final order
    int* stack;
    int* queue;
    int* alt;
    int num_cities; // The number of cities
}SalesMans;


int head, sp, tail, path;



void main(int argc,char* argv[]){

    SalesMans salesman;
    char* fileName;
    char logFile[4096];
    int i, j = 0;
    bool log = false;

    if(argc<2){
        perror("wrong command\n");
        exit(1);
    }

    fileName = argv[1];
    if(argc>=3){
        if(strcmp(argv[2],"log")==0){
            log = true;
            strcpy(logFile,argv[1]);
            strcat(logFile,".seq_log");
        }
    }


    // check the input file stat
    struct stat file;
    if(stat(fileName,&file)==-1){
        perror("Couldn't open file\n");
        exit(1);
    }

    FILE* fd;
    char buff[BUFFERSIZE];

    fd = fopen(fileName,"r");
    if(fd==NULL){
        perror("Couldn't open file\n");
        exit(1);
    }

    char *line;
    const char space[2] = " ";
    int countLine = 0, rows = 1, columns = 0;

    FILE* fptr;
    if(log){
        int dstFd;
        dstFd = creat(logFile,COPYMODE);
        if(dstFd==-1)
            printf("Cannot create %s",logFile);

        fptr = fopen(logFile,"w");
    }


    // Reading the input file
    while((line=fgets(buff,BUFFERSIZE,fd))!=NULL){
        //The first one is our input size
        rows = countLine;
        columns = 0;
        if(countLine==0){
            salesman.num_cities = atoi(line);
            /* Initialize */
            salesman.queue = calloc(salesman.num_cities,sizeof(int));
            salesman.stack = calloc(salesman.num_cities,sizeof(int));
            salesman.alt = calloc(salesman.num_cities,sizeof(int));
            salesman.order = calloc(salesman.num_cities,sizeof(int));
            salesman.matrix = (int**) malloc(salesman.num_cities*sizeof(int*));
            for(i=0;i<(int)salesman.num_cities;i++)
                salesman.matrix[i] = calloc(salesman.num_cities,sizeof(int));

        }else{
            char* dup = strdup(line);
            char* token;
            token = strtok(dup,space);

            for(i=0;i<countLine;i++){
                salesman.matrix[rows][columns] = atoi(token);
                salesman.matrix[columns][rows] = atoi(token);
                columns++;
                token = strtok(NULL,space);
            }
        }
        countLine++;
    }


    for(i=0;i<salesman.num_cities;i++){
        salesman.queue[i] = i;
    }
    path = MAX_DISTANCE;
    salesman.stack[0] = salesman.queue[0];
    salesman.alt[0] = 0;
    sp = 0;
    head = 0;
    tail = salesman.num_cities - 1;
    int tempPath = 0;

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    //Explore a branch of the factorial tree
    while(sp>=0){

        while(sp<salesman.num_cities-1 && tempPath<path){
            sp++;
            head++;
            if(head==salesman.num_cities)
                head = 0;

            salesman.stack[sp] = salesman.queue[head];
            tempPath = tempPath + salesman.matrix[salesman.stack[sp]][salesman.stack[sp-1]];
            salesman.alt[sp] = salesman.num_cities - sp - 1;
        }

        // save better solution
        if(tempPath<path){
            for(i=0;i<salesman.num_cities;i++){
                salesman.order[i] = salesman.stack[i] + 1;
            }
            path = tempPath;
        }


        // Leaving nodes when there is no branches
        while(salesman.alt[sp]==0 && sp>=0){
            tail++;
            if(tail==salesman.num_cities)
                tail = 0;
            salesman.queue[tail] = salesman.stack[sp];
            tempPath = tempPath - salesman.matrix[salesman.stack[sp]][salesman.stack[sp-1]];
            sp--;
        }

        if(log){
            fprintf(fptr,"%s","This path is pruned from the search: ");
            for(i=0;i<=sp;i++){
                fprintf(fptr,"%d ",salesman.stack[i]+1);
            }
            fprintf(fptr,"\n");
        }


        // If bottom of stack is reached then stop
        if(sp<0)
            break;
        tail++;
        if(tail==salesman.num_cities)
            tail = 0;

        salesman.queue[tail] = salesman.stack[sp];
        tempPath = tempPath - salesman.matrix[salesman.stack[sp]][salesman.stack[sp-1]];

        // Explore an alternate branch
        salesman.alt[sp] = salesman.alt[sp] - 1;
        head++;
        if(head==salesman.num_cities)
            head = 0;
        salesman.stack[sp] = salesman.queue[head];
        tempPath = tempPath + salesman.matrix[salesman.stack[sp]][salesman.stack[sp-1]];

    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


    if(log){
        fprintf(fptr,"%s","\nThe Path is: ");
    }
    for (i=0;i<salesman.num_cities;i++){
        if(i!=salesman.num_cities-1)
            printf("%d,",salesman.order[i]);
        else
            printf("%d\n",salesman.order[i]);
        if(log){
            fprintf(fptr,"%d ", salesman.order[i]);
        }
    }

    printf("Distance = %d\n",path);
    if(log){
        fprintf(fptr,"%s","\nDistance: ");

        fprintf(fptr,"%d\n",path);
    }

    //printf("The B&B took %f seconds to execute \n", cpu_time_used);
    if(log){
        int dstFd;
        dstFd = creat(logFile,COPYMODE);
        if(dstFd==-1)
            printf("Cannot create %s",logFile);

        FILE* fptr;
        fptr = fopen(logFile,"w");

        fclose(fptr);
        if(close(dstFd)==-1)
            printf("error closing files%s",logFile);
    }
}


















