#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <memory.h>
#include "mpi.h"

// to see if we have to stop revieving files from the previous proccessor
#define SENTINEL 0.0

int main(int argc, char **argv)
{
    
    FILE *pInfile;  // pointer for the file
    char c;   
    float a_coeff, b_coeff, c_coeff, disc;  // to store a,b,c coeffiecients and the d value
    float x1r, x2r; // to store the roots
    int fileElementCount = 0; // total number of equations for which roots we have to found
    int counter = 0 ;
    int my_rank; // rank of my proccessor
    int p; // total proccessors
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    // WRITE PART(a) HERE
    
    switch (my_rank){
    case 0:{  // if rank 0 read file and calculate d values and send it to rank 2
    // CONTINUE WITH PART (a) HERE
        
        pInfile = fopen("a.txt","r");
		fscanf(pInfile, "%d", &fileElementCount);  // rerad total number of elements in file
		MPI_Send(&fileElementCount, 1, MPI_INT, (p - 1), 0, MPI_COMM_WORLD);  // send number of elements to the last proccessor
		int start = 0;
		while(1) {  // to skip the line that has legend text a b c
   
            c = fgetc(pInfile);
            if(c == '\n'){
                if(start == 0){
                    start = 1;  
                }
                else{
                    break;  
                }
            }
        } 
		while(counter < fileElementCount)
		{
		        
            fscanf(pInfile,"%f", &a_coeff); // get a value
            fscanf(pInfile,"%f", &b_coeff); // get b value
            fscanf(pInfile,"%f", &c_coeff); // get c value
            disc = b_coeff*b_coeff - 4*a_coeff*c_coeff; // calculate d
            
            //send value to processor 1
			MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(&b_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
			counter++;
		}
		fclose(pInfile); // close file
		pInfile = NULL;
		a_coeff = SENTINEL;
		MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD); // tell processor 1 to stop recieving
		break;

    }
    case 1:{ // if proccessor 1 check value of d and caluclate the roots accordingly and send them to proccessor 2
    // WRITE PART (b) HERE
        do
			{ 
				MPI_Recv(&a_coeff, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status); // recieve a value and check if we have to stop recieving from the processor 0
				if(a_coeff != SENTINEL)
				{ 
					MPI_Recv(&b_coeff, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status); // revieve b value
					MPI_Recv(&disc, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status); // recieve d value
					if(disc < 0) // if d value is less than 0 calculate the real and imaginary value of the root
					{
					    x1r = -b_coeff/(double)(2*a_coeff);
					    x2r = sqrt(-disc)/(2*a_coeff);
					    
					}
					else { // if d >= 0 calculate normal roots
					    x1r = (-b_coeff + sqrt(disc))/(2*a_coeff);
                        x2r = (-b_coeff - sqrt(disc))/(2*a_coeff);
					}
					
					// send root 1 root2 and d value to proccessor 2
					MPI_Send(&x1r, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD); 
			        MPI_Send(&x2r, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
			        MPI_Send(&disc, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
				}
				else
				{
					a_coeff = SENTINEL; 
				}
			} while (a_coeff != SENTINEL);
			break;
    }
    case 2:{
    // WRITE PART (c) HERE
        MPI_Recv(&fileElementCount, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status); // recieve number of rows from proccessor 0 
        pInfile = fopen("output.txt","w"); // open file to write the values in it
        fprintf(pInfile, "%d" ,fileElementCount); // wrote number of rows in file
        fprintf(pInfile, "\nx1\t\tx2\tx1_real\tx1_img\tx2_real\tx2_img"); // write the legen in the file
        
        while(counter <fileElementCount){ 
        
            // recieve root1 root2 and d value from proccessor 1
            MPI_Recv(&x1r, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&x2r, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
			if(disc<0){ // if d value is less than 0 then x1r is the real x1 and x2 value and x2r is the imaginary x1 and x2 value
			    fprintf(pInfile, "\n\t\t\t%.2f\t%.2f\t%.2f\t%.2f",x1r,x2r,x1r,x2r);
			}
			else{ // if d >=0 x1 and x2 are roots 
			    fprintf(pInfile, "\n%.2f\t%.2f\t\t\t\t",x1r,x2r);
			}
			counter++;
			
        }
        fclose(pInfile); // close the file
		pInfile = NULL;
		printf("See result in output.txt");
        
        break;
    }
}
MPI_Finalize(); //end
return 0;
}
