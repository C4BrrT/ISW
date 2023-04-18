#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int sudoku[5000][5000];
int repeat_l=0;
int repeat_c=0;
int repeat_sub=0;

typedef struct{
    int size_s;
    int l;
    int c;
    int l_sub;
    int c_sub;
    int no;
}matrix;

void *verify_line(void *ptr);
void *verify_column(void *ptr);
void *verify_sub(void *ptr);

int main(int argc, char *argv[]){
    FILE *inputp, *outp;
    inputp = fopen(argv[1], "r");
    int size, ver_size, subl, subc, correct=1, space_count=0;
    char line_size[10000];

    if (argc != 2){ 
	    printf("Invalid number of parameters\n");
        exit(EXIT_FAILURE);
	}
	else if (inputp == NULL){ 
		printf("Error to read file\n");
        exit(EXIT_FAILURE);
	}

    fscanf(inputp, "%dx%d", &size, &ver_size);
	fscanf(inputp, "%dx%d", &subl, &subc);
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
		    fscanf(inputp, "%d ", &sudoku[i][j]);
            if(sudoku[i][j]>size || sudoku[i][j]<1){
                printf("File out of format\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if(fgetc(inputp)!=EOF){
        printf("File out of format\n");
        exit(EXIT_FAILURE);
    }

    fseek(inputp, 0, SEEK_SET);
    for(int i=0; i<2; i++){
        fgets(line_size, 10000, inputp);
        for(int j=0; j<strlen(line_size); j++){
            if(line_size[j]==' '){
                printf("File out of format\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    for(int i=0; i<size; i++){
        fgets(line_size, 10000, inputp);
        for(int j=0; j<strlen(line_size); j++){
            if(line_size[j]==' '){
                space_count++;
            }
            if(i==size-1 && line_size[j]=='\n'){
                printf("File out of format\n");
                exit(EXIT_FAILURE);
            }
        }
        if(space_count!=size-1){
            printf("File out of format\n");
            exit(EXIT_FAILURE);
        }
        space_count=0;
    }

    fclose(inputp);

    if(subl*subc!=size){
        printf("File out of format\n");
        exit(EXIT_FAILURE);
    }
    if(size != ver_size){
        printf("File out of format\n");
        exit(EXIT_FAILURE);
    }

    pthread_t thread[size*size*2];
    int count_t=0;

    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            matrix *sudoku_main = malloc(sizeof (matrix));
            sudoku_main->l = i;
            sudoku_main->c = j;
            sudoku_main->size_s = size;
            sudoku_main->l_sub = subl;
            sudoku_main->c_sub = subc;
            sudoku_main->no = sudoku[i][j];
            pthread_create(&thread[count_t], NULL, verify_line, (void *) sudoku_main);
            count_t++;
            pthread_create(&thread[count_t], NULL, verify_column, (void *) sudoku_main);
            count_t++;
            if(i==0 && j==0){
                verify_sub((void *) sudoku_main);
            }
        }
    }   

    for(int i=0; i<count_t; i++){
        pthread_join(thread[i], NULL);
    }

    //printf("%d %d %d %d", count_t, repeat_l, repeat_c, repeat_sub);

    outp = fopen("sudoku_ccbc.out", "w");

    if(repeat_l==0 && repeat_c==0 && repeat_sub==0){
        fprintf(outp, "SUCCESS");
    }
    else{
        fprintf(outp, "FAIL");
    }
    
    fclose(outp);

    return 0;
}

//no sempre 1° da linha
void *verify_line(void *ptr){
    matrix *sudoku_f1 = (matrix *) ptr;
    int line = sudoku_f1->l;
    for(int i=1; i<sudoku_f1->size_s; i++){
        if(sudoku_f1->no == sudoku[line][i] && sudoku_f1->c != i){
            repeat_l++;
        }
    }
    return NULL;
    free(ptr);
}

void *verify_column(void *ptr){
    matrix *sudoku_f2 = (matrix *) ptr;
    int column = sudoku_f2->c;
    for(int i=1; i<sudoku_f2->size_s; i++){
        if(sudoku_f2->no == sudoku[i][column] && sudoku_f2->l != i){
            repeat_c++;
        }
    }
    return NULL;
    free(ptr);
}

void *verify_sub(void *ptr){
    matrix *sudoku_f3 = (matrix *) ptr;
    int size_f3 = sudoku_f3->size_s;
    int line = sudoku_f3->l;
    int col = sudoku_f3->c;
    int sub_line = sudoku_f3->l_sub;
    int sub_col = sudoku_f3->c_sub;
    int count_line = 0;
    int count_col = 0;
    int num_check = 0;

    if(sub_line<=sub_col){
        for(int i=0; i<size_f3; i++){
            for(int j=0; j<size_f3; j++){
                num_check=i+1;
                for(int k=count_line; k<count_line+sub_line; k++){
                    for(int l=count_col; l<count_col+sub_col; l++){
                        if(sudoku[j][k]==num_check){
                            repeat_sub++;
                        }
                    }
                }
            }
            if(repeat_sub==size_f3){
                repeat_sub=0;
            }
            else{
                repeat_sub = 1;
                break;
            }
            if(count_line+sub_line<size_f3){
                count_line+=sub_line;
            }
            else{
                count_line=0;
                count_col+=sub_col;
            }
        }
    }
    else{
        for(int i=0; i<size_f3; i++){
            for(int j=0; j<size_f3; j++){
                num_check=i+1;
                for(int k=count_col; k<count_col+sub_col; k++){
                    for(int l=count_line; l<count_line+sub_line; l++){
                        if(sudoku[l][k]==num_check){
                            repeat_sub++;
                        }
                    }
                }
            }
            if(repeat_sub==size_f3){
                repeat_sub=0;
            }
            else{
                repeat_sub = 1;
                break;
            }
            if(count_line+sub_line<size_f3){
                count_line+=sub_line;
            }
            else{
                count_line=0;
                count_col+=sub_col;
            }
        }
    }

    return NULL;
    free(ptr);
}
