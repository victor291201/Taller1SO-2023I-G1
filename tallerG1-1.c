#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <time.h>
#include <string.h>

int main( int argc, char **argv ){
    
    FILE *ptr_file; int c=0,n=0;char l;
    ptr_file = fopen("comands.txt","rt");
    if (ptr_file == NULL)
    {
        puts("Error: No se encontró el archivo.");

    }
    while((c = fgetc(ptr_file)) != EOF){
        if( c == '\n')
            n++;
        putchar(c);
    }
    n ++;
    printf ("\nNúmero de líneas del archivo: %d\n",n);
    fclose(ptr_file);

    //declarando los datos que utilizaremos para guardar los pids
    pid_t pidp = getpid();
    pid_t* pidsh = (pid_t*) malloc(sizeof(pid_t)*n);
    for(int i  = 0; i<n;i++){
        pidsh[i]=-1;
    }
    //declarando las tuberias
    //creando las tuberias
    int pps[n][2];
    for(int i = 0;i<n;i++){
        pipe(pps[i]);
    }
    //declarando los bufers de lectura y escritura
   char Buffer_out[1024];
   char Buffer_in[1024];
    //creando los hijos 
    //abriendo el archivo de texto en una matriz
    char **matrix = NULL;
    char* line =NULL;
    int ind=0;
    int max_line = 1024;
    line = (char*) malloc(max_line* sizeof(char));
    matrix = (char**) malloc(max_line* sizeof(char*));
    FILE* fp = fopen("comands.txt","r");
    if(fp == NULL){perror("error al abrir el archivo \n");exit(EXIT_FAILURE);}
    while (fgets(line, max_line*sizeof(char), fp) ){
        matrix[ind] = line;
        line = (char*) malloc(max_line * sizeof(char));
        ind++;
    }
    fclose(fp);
    
    int h;
    for(h=0;h<n;h++){
        pidsh[h]=fork();
        if(pidsh[h] == 0 ){
            break;
        }
    }
    //creando la logica para cada proceso
    if(pidp == getpid()){
        //cerrar pipes escritura
        //logica del padre
        for(int i=0;i<n;i++){
            wait(NULL);
        }
        for(int i=0;i<n;i++){
            read(pps[i][0], Buffer_in, sizeof(Buffer_in));
            printf("la salida del hijo [%d] es \n %s \n",i,Buffer_in);
        }
        //cerrando las pipes restantes
        //terminando el programa
        printf("terminando el padre .... \n");
        
    }else{
        int tub[2];
        pid_t child;
        if(pipe(tub) == -1){return -1;}
        child = fork();
        switch (child){
        case -1:
            return -1;
        case 0:
            close(tub[0]);
            dup2(tub[1],STDOUT_FILENO);
            execl("/bin/sh","sh","-c",matrix[h],NULL);
            return 0;
        default:
            wait(NULL);
            read(tub[0], Buffer_in, sizeof(Buffer_in));
            strcpy(Buffer_out, Buffer_in);
            write(pps[h][1],Buffer_out,sizeof(Buffer_out));
            break;
        }

    }
    return 0;
}