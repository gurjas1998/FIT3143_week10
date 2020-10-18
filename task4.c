#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <memory.h>
#include "mpi.h"

#define SENTINEL 0.0

int main(int argc, char **argv)
{
    int a = 0;
    printf("%d",a);
    FILE *pInfile;
    float a_coeff, b_coeff, c_coeff, disc;
    float x1r, x2r;
    int fileElementCount = 0;
    int counter = 0 ;
    int my_rank;
    int p;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    // WRITE PART(a) HERE
    
    switch (my_rank){
    case 0:{
    // CONTINUE WITH PART (a) HERE
        printf("1");
        pInfile = fopen("a.txt","r");
		fscanf(pInfile, "%d", &fileElementCount);
		printf("%d",fileElementCount);
		MPI_Send(&fileElementCount, 1, MPI_INT, (p - 1), 0, MPI_COMM_WORLD);
		while(counter < fileElementCount)
		{
		        
            fscanf(pInfile,"%f", &a_coeff);
            fscanf(pInfile,"%f", &b_coeff);
            fscanf(pInfile,"%f", &c_coeff);
            disc = b_coeff*b_coeff - 4*a_coeff*c_coeff;
			MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(&b_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
			counter++;
			printf("sending to 1");
		}
		fclose(pInfile);
		pInfile = NULL;
		a_coeff = SENTINEL;
		MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
		break;

    }
    case 1:{
    // WRITE PART (b) HERE
    printf("2");
        do
			{ 
				MPI_Recv(&a_coeff, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
				if(a_coeff != SENTINEL)
				{ 
					MPI_Recv(&b_coeff, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
					MPI_Recv(&disc, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
					if(disc < 0)
					{
					    x1r = -b_coeff/(double)(2*a_coeff);
					    x2r = sqrt(-disc)/(2*a_coeff);
					    
					}
					else {
					    x1r = (-b_coeff + sqrt(disc))/(2*a_coeff);
                        x2r = (-b_coeff - sqrt(disc))/(2*a_coeff);
					}
					MPI_Send(&x1r, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
			        MPI_Send(&x2r, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
			        MPI_Send(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				}
				else
				{
					a_coeff = SENTINEL;
				}
				printf("sending to ");
			} while (a_coeff != SENTINEL);
			break;
    }
    case 2:{
    // WRITE PART (c) HERE
        printf("3");
        MPI_Recv(&fileElementCount, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        pInfile = fopen("output.txt","w");
        fprintf(pInfile, "%d" ,fileElementCount);
        fprintf(pInfile, "\nx1\tx2\tx1_real\tx1_img\tx2_real\tx2_img");
        
        while(counter <fileElementCount){
        
            MPI_Recv(&x1r, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&x2r, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&disc, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
			if(disc<0){
			    fprintf(pInfile, "\n\t\t%.2f\t%.2f\t%.2f\t%.2f",x1r,x2r,x1r,x2r);
			}
			else{
			    fprintf(pInfile, "\n%.2f\t%.2f\t\t\t\t",x1r,x2r);
			}
			counter++;
			printf("done");
        }
        fclose(pInfile);
		pInfile = NULL;
        
        break;
    }
}
MPI_Finalize();
return 0;
}

