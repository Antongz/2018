/*
 * =====================================================================================
 *
 *       Filename:  tsp_parallel.c
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
#include <mpi.h>
#include <time.h>
#include <assert.h>

#define MAX_DISTANCE 9999
#define BUFFERSIZE 4096
#define COPYMODE 0644


int** matrix;   // Store the distances between cities
int* order;     // Store the final order
int* stack;
int* alt;
int num_cities; // The number of cities


int i;

/*The root process will read the input file and setup the variables*/
void readFile(char* fileName){
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


    // Reading the input file
    while((line=fgets(buff,BUFFERSIZE,fd))!=NULL){
        //The first one is our input size
        rows = countLine;
        columns = 0;
        if(countLine==0){
            num_cities = atoi(line);
            /* Initialize */
            MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&stack);
            MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&alt);
            MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&order);
            MPI_Alloc_mem(sizeof(int*)*(num_cities),MPI_INFO_NULL,&matrix);
            for(i=0;i<(int)num_cities;i++)
                MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&matrix[i]);

        }else{
            char* dup = strdup(line);
            char* token;
            token = strtok(dup,space);

            for(i=0;i<countLine;i++){
                matrix[rows][columns] = atoi(token);
                matrix[columns][rows] = atoi(token);
                columns++;
                token = strtok(NULL,space);
            }
        }
        countLine++;
    }


    for(i=0;i<num_cities;i++){
        matrix[i][i] = MAX_DISTANCE;
    }
    stack[0] = 0;
    alt[0] = 0;
}


void main(int argc,char* argv[]){

    if(argc<2){
        perror("wrong command\n");
        exit(1);
    }

    MPI_Init(&argc,&argv);  /*start MPI*/
    int head, sp, tail, procId, numProcs;

    char* fileName;
    char logFile[4096];
    int j;
    bool log = false;
    fileName = argv[1];


    const int MY_MSG_TAG = 0;
    const int START_TAG = 1;
    MPI_Status Stat;
    MPI_Comm world_group;   /*declare a communicator ptr*/
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);   /* get number of processes */
    MPI_Comm_rank(MPI_COMM_WORLD, &procId); /* get current process id */

    if(argc>=3){
        if(strcmp(argv[2],"log")==0){
            log = true;
            strcpy(logFile,argv[1]);
            strcat(logFile,".parallel_log");
        }
    }

    FILE* fptr;
    MPI_File fh;
    sp = 0;
    head = 0;
    tail = num_cities - 1;     //because the salesman doesnt return back
    int tempPath = 0;
    int** start_queue;

    /*Root process will read the file and broadcast the values to other processes*/
    if(procId==0){
        readFile(fileName);
        MPI_Bcast(&num_cities,1,MPI_INT,0,MPI_COMM_WORLD);

        MPI_Alloc_mem(sizeof(int*)*(num_cities-1),MPI_INFO_NULL,&start_queue);
        for(i=0;i<(int)num_cities-1;i++)
            MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&start_queue[i]);
        int itr = 1;
        for(i=0;i<num_cities-1;i++){
            start_queue[i][0] = 0;
            start_queue[i][1] = itr;
            itr++;
        }
        for(i=0;i<num_cities-1;i++){
            for(j=2;j<num_cities;j++){
                if(start_queue[i][1]<j){
                    start_queue[i][j] = j;
                }else{
                    start_queue[i][j] = j-1;
                }
            }
        }

        if(log){
            int dstFd;
            dstFd = creat(logFile,COPYMODE);
            if(dstFd==-1)
                printf("Cannot create %s",logFile);

            fptr = fopen(logFile,"w");
            MPI_File_open(MPI_COMM_SELF,logFile,MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL,&fh);
        }
    }else{
        MPI_Bcast(&num_cities,1,MPI_INT,0,MPI_COMM_WORLD);
        MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&stack);
        MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&alt);
        MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&order);
        MPI_Alloc_mem(sizeof(int*)*(num_cities),MPI_INFO_NULL,&matrix);
        for(i=0;i<(int)num_cities;i++)
            MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&matrix[i]);
        MPI_Alloc_mem(sizeof(int*)*(num_cities-1),MPI_INFO_NULL,&start_queue);
        for(i=0;i<(int)num_cities-1;i++)
            MPI_Alloc_mem(sizeof(int)*(num_cities),MPI_INFO_NULL,&start_queue[i]);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for(i=0;i<num_cities;i++)
        MPI_Bcast(&matrix[i][0],num_cities,MPI_INT,0,MPI_COMM_WORLD);
    for(i=0;i<num_cities-1;i++)
        MPI_Bcast(&start_queue[i][0],num_cities,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&stack[0],num_cities,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&alt[0],num_cities,MPI_INT,0,MPI_COMM_WORLD);

    int result_buffer[num_cities+1];
    int iterator = (num_cities-1) / (numProcs);
    int next = 0;
    int base = (int)procId * iterator;
    result_buffer[num_cities] = MAX_DISTANCE;
    int *final_buffer = NULL;
    if(procId==0){
        MPI_Alloc_mem(sizeof(int)*((num_cities+1)*numProcs),MPI_INFO_NULL,&final_buffer);
        assert(final_buffer!=NULL);
    }


    /*Use MPI_Wtime to count the MPI runtime*/
    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    //Explore a branch of the factorial tree
    while(sp>=0&&((next<iterator)||(base+next==num_cities-2))){
        /*Put the next city to our stack, also if the current path
         * is already bigger than the shortest path we've got, do
         * not explore the branch tree. Which is also the pruning behaviour*/
        while(sp<num_cities-1 && tempPath< result_buffer[num_cities]){
            sp++;
            head++;
            if(head==num_cities)
                head = 0;

            stack[sp] = (int)start_queue[base+next][head];
            tempPath = tempPath + matrix[stack[sp]][stack[sp-1]];
            alt[sp] = num_cities - sp - 1;
        }

        //Save better solution
        if(tempPath< result_buffer[num_cities]){
            result_buffer[num_cities] = tempPath;

            for(i=0;i<num_cities;i++){
                order[i] = stack[i] + 1;
                result_buffer[i] = (int)stack[i] + 1;
            }
        }

        //Leaving nodes when there is no branches
        while(alt[sp]==0 && sp>=0){
            tail++;
            if(tail==num_cities)
                tail = 0;
            start_queue[base+next][tail] = stack[sp];
            tempPath = tempPath - matrix[stack[sp]][stack[sp-1]];
            sp--;
        }

        //If bottom of stack is reached then stop
        if(sp<0)
            break;
        tail++;
        if(tail==num_cities)
            tail = 0;

        start_queue[base+next][tail] = stack[sp];
        tempPath = tempPath - matrix[stack[sp]][stack[sp-1]];

        //Explore an alternate branch
        alt[sp] = alt[sp] - 1;
        head++;
        if(head==num_cities)
            head = 0;
        stack[sp] = start_queue[base+next][head];
        tempPath = tempPath + matrix[stack[sp]][stack[sp-1]];

        /*If we have already explored this branch tree, processes communicate
         * one another to gather and synchronize the result buffer*/
        if(sp==1){
            tail = num_cities - 1;
            sp = 0;
            head = 0;
            next++;
            tempPath = 0;
            alt[0] = 0;

            if(!(num_cities%2==0&&base+next==num_cities-2))
                MPI_Gather(&result_buffer,num_cities+1,MPI_INT,final_buffer,(num_cities+1),MPI_INT,0,MPI_COMM_WORLD);
            //MPI_Barrier(MPI_COMM_WORLD);
            if(procId==0){
                for(i=0;i<numProcs;i++){
                    if(result_buffer[num_cities]>final_buffer[num_cities+i*(num_cities+1)]){
                        for(j=0;j<num_cities;j++)
                            result_buffer[j] = final_buffer[(num_cities+1)*i+j];
                        result_buffer[num_cities] = final_buffer[num_cities+i*(num_cities+1)];
                    }
                }
                MPI_Bcast(&result_buffer,num_cities+1,MPI_INT,0,MPI_COMM_WORLD);
            }else{
                if(!(num_cities%2==0&&base+next==num_cities-2))
                    MPI_Bcast(&result_buffer,num_cities+1,MPI_INT,0,MPI_COMM_WORLD);
            }

        }

    }


    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    MPI_Gather(&result_buffer,num_cities+1,MPI_INT,final_buffer,(num_cities+1),MPI_INT,0,MPI_COMM_WORLD);
    if(procId==0){
        for(i=0;i<numProcs;i++){
            if(result_buffer[num_cities]>final_buffer[num_cities+i*(num_cities+1)]){
                for(j=0;j<num_cities;j++)
                    result_buffer[j] = final_buffer[(num_cities+1)*i+j];
                result_buffer[num_cities] = final_buffer[num_cities+i*(num_cities+1)];
            }
        }
    }


   if(procId==0){
        if(log){
            fprintf(fptr,"%s","\nThe Path is: ");
        }
        for (i=0;i<num_cities;i++){
            if(i!=num_cities-1){
                printf("%d,",result_buffer[i]);
                if(log){
                    fprintf(fptr,"%d,",result_buffer[i]);
                    //MPI_File_write(fh,",",strlen(","),MPI_CHAR,&Stat);
                }
            }else{
                printf("%d\n",result_buffer[i]);
                if(log)
                    fprintf(fptr,"%d\n,",result_buffer[i]);
            }

        }


        printf("Distance = %d\n",result_buffer[num_cities]);
        if(log){
            fprintf(fptr,"%s","\nDistance: ");
            fprintf(fptr,"%d\n",result_buffer[num_cities]);
            //MPI_File_write(fh,result_buffer,num_cities,MPI_INT,&Stat);
            //MPI_File_write(fh,"Distance = ",strlen("Distance = "),MPI_CHAR,&Stat);
            //MPI_File_write(fh,result_buffer[i],1,MPI_INT,&Stat);
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
        //printf("Runtime = %f\n", end-start);
    }



    MPI_File_close(&fh);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

}


