
/*************************************************
	NAME: NWANKWO CHUKWUEBUKA JUSTIN
	DATE: 12/5/2016

*************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void display_save_memory(FILE * file, int * memory,char * str,int size,int option);
void display_save_float(FILE * file, float * memory,char * str,int size,int option);
void display_save_double(FILE * file, double * memory,char * str,int size,int option);


int main(void){
	FILE * fp;     //hexFile pointer
	FILE * openFile;  //saveFile pointer

//	fp=fopen("PA4_example.hex","r"); //opens the hex file for reading
	fp=fopen("PA4_example.hex","r");
	openFile=fopen("Register_values.txt","w+");  

	if(fp==NULL || openFile==NULL){
		printf("Error: problem opening files\n");
		exit(1);
	}

	int * instructionMem=(int *)calloc(4096,sizeof(int));
	int * registerValues=(int *)calloc(32,sizeof(int));
	int * dataMem=(int *)calloc(4096,sizeof(int));

	//start
	float * floatRegister = (float *)calloc(32,sizeof(float));
	double * doubleRegister = (double *)calloc(16,sizeof(double));


	int lowRegister,highRegister;
	int FPcond=0;

	long long temp; //used to store 64 bits multiplication

	/*remove
	int b =0;
	dataMem[0]=8;
	dataMem[2]=12;
	for(b=0; b<32; b++)
		floatRegister[b] = 3.5;

	for(b=0; b<16; b++)
		doubleRegister[b] = 5.5;
	*/

	display_save_memory(openFile,registerValues,"Reg. ",32,0);
	display_save_float(openFile,floatRegister, "Flt. ",32,0);
	display_save_double(openFile,doubleRegister, "Dbl. ",16,0);
	display_save_memory(openFile,dataMem,"Data. ",4096,0);

	//Stores contents of fp file in instructionMem array
	int i=0;
	while(fscanf(fp,"%x",&instructionMem[i])!=EOF)
		i++;

	int numberOfInstructions=i;

	int opcode,funct,rs,rt,rd,immediate,immediate_sign,shamt;
	unsigned int hexValue;   //Stores each hex values

	int pc=0;  //program counter

	i=0; //resets loop counter
	while(pc < numberOfInstructions){
		hexValue=instructionMem[pc];
		opcode=(hexValue>>26);  //gets the opcode (R  or I format)
		++pc;
		if(opcode==0){  //R format
			funct=(hexValue & 0x3f);
			rs=(hexValue >> 21) & 0x1f;
			rt=(hexValue >> 16) & 0x1f;
			rd=(hexValue >> 11) & 0x1f;
			shamt = (hexValue >> 6) & 0x1f;

			if(funct==32){  //add
				registerValues[rd]=registerValues[rs]+registerValues[rt];
			}

			else if(funct==33){ //add unsigned
				registerValues[rd]=registerValues[rs]+registerValues[rt];
			}

			else if(funct==34){  //sub
				registerValues[rd]=registerValues[rs]-registerValues[rt];
			}

			else if(funct==35){  //sub unsigned
				registerValues[rd]=registerValues[rs]-registerValues[rt];
			}

			else if(funct==42){ //slt
				if(registerValues[rs]<registerValues[rt])
					registerValues[rd]=1;
				else
					registerValues[rd]=0;
			}

			else if(funct==43){ //sltu
				if(registerValues[rs] < registerValues[rt])
					registerValues[rd]=1;
				else
					registerValues[rd]=0;
			}

			else if(funct == 0){ //sll
				registerValues[rd]= registerValues[rt] << shamt;
			}

			else if(funct == 2){ //srl
				registerValues[rd] = registerValues[rt] >> shamt;
			}

			else if(funct == 36){ //and
				registerValues[rd] = registerValues[rs] & registerValues[rt];
			}

			else if(funct == 37){ //or
				registerValues[rd] = registerValues[rs] | registerValues[rt];
			}

			else if(funct == 39){ //nor
				registerValues[rd] = ~(registerValues[rs] | registerValues[rt]);
			}

			else if(funct == 24){ //mult
				temp = registerValues[rs] * registerValues[rt];
				highRegister = temp >> 32;
				lowRegister  = temp & 0xffffffff;
			}

			else if(funct == 16){ //mfhi
				registerValues[rd] = highRegister;
			}

			else if(funct == 18){ //mflo
				registerValues[rd] = lowRegister;
			}

			else if(funct == 26){ //div
				lowRegister  = registerValues[rs] / registerValues[rt];
				highRegister = registerValues[rs] % registerValues[rt];
			}

			else if(funct == 27){ //divu
				lowRegister  = registerValues[rs] / registerValues[rt];
				highRegister = registerValues[rs] % registerValues[rt];
			}

		}

		//fLOATING POINT OPERATIONS
		else if(opcode == 17){  //FR and FI
			int fmt = (hexValue >> 21) & 0x1f;
			int ft  = (hexValue >> 16) & 0x1f;
                        int fs  = (hexValue >> 11) & 0x1f;
                        int fd  = (hexValue >> 6)  & 0x1f;
                        int funct = hexValue & 0x3f;
 

			immediate=hexValue & 0xffff;  //gets the constant value
			immediate_sign=((immediate & 0x8000) >> 15);  //gets the sign bit of the constant


                        if(immediate_sign != 0){  //computes if the constant is negative
                               immediate=immediate | 0xffff0000;
                         }

			if(fmt == 16){ //Single FR
				if(funct == 0){  //add.s
					floatRegister[fd] = floatRegister[fs] + floatRegister[ft];
				}

				else if(funct == 1){  //sub.s
					floatRegister[fd] = floatRegister[fs] - floatRegister[ft];
				}

				else if(funct == 2){  //mult.s
					floatRegister[fd] = floatRegister[fs] * floatRegister[ft];
				}

				else if(funct == 3){  //div.s
					floatRegister[fd] = floatRegister[fs] / floatRegister[ft];
			 	}

				else if(funct == 50){ //c.eq.s
					if(floatRegister[fs] == floatRegister[ft])
						FPcond = 1;
					else
						FPcond = 0;
				}

				else if(funct == 60){ //c.lt.s
					if(floatRegister[fs] < floatRegister[ft])
						FPcond = 1;
					else
						FPcond = 0;
				}

				else if(funct == 62){ //c.le.s
					printf("here\n");
					if(floatRegister[fs] <= floatRegister[ft])
						FPcond = 1;
					else
						FPcond = 0;
				}

				else if(funct == 33) { //cvt.d.s
					doubleRegister[fd/2] = (double)floatRegister[fs];
				}

				else if(funct == 36){ //cvt.w.s
					floatRegister[fd] = (int)floatRegister[fs];
				}


			}

			else if(fmt == 17){ //Double FR
				if(funct == 0){ //add.d
					doubleRegister[fd/2] = doubleRegister[fs/2] + doubleRegister[ft/2];
				}

				else if(funct == 1){  //sub.d
					doubleRegister[fd/2] = doubleRegister[fs/2] - doubleRegister[ft/2];
				}

				else if(funct == 2){ //mult.d
					doubleRegister[fd/2] = doubleRegister[fs/2] * doubleRegister[ft/2];
				}

				else if(funct == 3){ //div.d
					doubleRegister[fd/2] = doubleRegister[fs/2] / doubleRegister[ft/2];
				}

				else if(funct == 50){ //c.eq.d
					if(doubleRegister[fs/2] == doubleRegister[ft/2])
						FPcond = 1;
					else
						FPcond = 0;
				}

				else if(funct == 60){ //c.lt.d
					if(doubleRegister[fs/2] < doubleRegister[ft/2])
						FPcond = 1;
					else
						FPcond = 0;
				}

				else if(funct == 62){ //c.le.d
					if(doubleRegister[fs/2] <= doubleRegister[ft/2])
						FPcond = 1;
					else
						FPcond = 0;
				}

				else if(funct == 32){ //cvt.s.d
					floatRegister[fd] = (float)doubleRegister[fs/2];
				}

				else if(funct == 36){ //cvt.w.d
					floatRegister[fd] = (int)doubleRegister[fs/2];
				}
			}

			else if(fmt == 8){  //FI
				if(ft==1 && FPcond == 1){ //bc1t
					pc = pc + immediate;
				}

				if(ft==0 && FPcond == 0){ //bc1f
					pc = pc + immediate;
				}
			}

			else if(fmt == 20 && funct == 32){  //cvt.s.w
				floatRegister[fd] = (float)floatRegister[fs];
			}

			else if(fmt == 20 && funct == 33){ //cvt.d.w
				doubleRegister[fd/2] = (double)floatRegister[fs];
			}



		}


		else{  //I and j format
			rt=(hexValue >> 16) &  0x1f;  //gets the destination register index
			rs=(hexValue >> 21) &  0x1f;  //gets the source register index
			immediate=hexValue & 0xffff;  //gets the constant value
			immediate_sign=((immediate & 0x8000) >> 15);  //gets the sign bit of the constant


                        if(immediate_sign != 0){  //computes if the constant is negative
                               immediate=immediate | 0xffff0000;
                         }

			if(opcode == 8 || opcode == 9 ){ //addi and addiu
				registerValues[rt]=registerValues[rs]+immediate;
				}

			else if(opcode == 10){ //slti
				if(registerValues[rs]<immediate)
					registerValues[rt]=1;
				else
					registerValues[rt]=0;
			}

			else if(opcode == 5){ //bne
				if(registerValues[rt]!=registerValues[rs])
					pc = pc + immediate;
			}

			else if(opcode == 4){ //beq
				if(registerValues[rt]==registerValues[rs])
					pc = pc + immediate;
			}


/*
			else if(opcode == 2){  //jump (j format)
				int jumpindex = (hexValue & 0x3ffffff);
				pc=jumpindex;
			}

			else if(opcode == 35){  //load word
				rt=rt+immediate;
				registerValues[rs]=dataMem[rt/4];
			}

			else if(opcode == 43){  //save word
				rt=rt+immediate;
				dataMem[rt/4]=registerValues[rs];
			}
*/

			else if(opcode == 13){ //ori
				registerValues[rt] = registerValues[rs] | immediate;
			}

			else if(opcode == 15){ //lui
				registerValues[rt] = immediate;
				registerValues[rt] = registerValues[rt] << 16;
			}

			else if(opcode == 31){ //lb
				rt = rt + immediate;
				registerValues[rs] = (dataMem[rt/4] >> 24);
			}

			else if(opcode == 36){ //lbu
				rt=rt + immediate;
				registerValues[rs] = (dataMem[rt/4] >> 24);
			}

			else if(opcode == 40){ //sb
				rt = rt + immediate;
				dataMem[rt/4] = (registerValues[rs] >> 24);
			}

			//start  float I
			else if(opcode == 49){ //lwc1
				rs = rs + immediate;
				floatRegister[rt] = dataMem[rs/4];
			}

			else if(opcode == 53){ //ldc1 **
				rs = rs + immediate;
				doubleRegister[rt/2] = dataMem[rs/4];
			}

			else if(opcode == 57){ //swc1
				rs = rs + immediate;
				dataMem[rs/4] = floatRegister[rt];
			}

			else if(opcode == 61){ //sdc1 **
				rs = rs + immediate;
				dataMem[rs/4] = doubleRegister[rt/2];
			}

		}
	}


	display_save_memory(openFile,registerValues,"Reg. ",32,1);
	display_save_float(openFile,floatRegister, "Flt. ",32,1);
	display_save_double(openFile,doubleRegister, "Dbl. ",16,1);
	display_save_memory(openFile,dataMem,"Data. ",4096,1);

	fclose(openFile);
	fclose(fp);


}

void display_save_memory(FILE * file, int * memory, char * str, int size,int option){
	if(option==0){
		fprintf(file,"Initial %s Values: . . .\n",str);
		printf("Initial %s Values: . . . \n",str);
	}
	else{
		fprintf(file,"Final %s Values: . . .\n",str);
		printf("Final %s Values: . . . \n",str);
	}
	int i;
	for(i=0; i<size; i++){
		fprintf(file,"%s %4d: %d\t",str,i,memory[i]);
		printf("%s %4d: %d\t",str,i,memory[i]);
		if((i+1)%8==0){
			fprintf(file,"\n");
			printf("\n");
		}
	}

	fprintf(file,"\n\n\n");
	printf("\n\n\n");

}


void display_save_float(FILE * file, float * memory, char * str, int size,int option){
        if(option==0){
                fprintf(file,"Initial %s Values: . . .\n",str);
                printf("Initial %s Values: . . . \n",str);
        }
        else{
                fprintf(file,"Final %s Values: . . .\n",str);
                printf("Final %s Values: . . . \n",str);
        }
        int i;
        for(i=0; i<size; i++){
                fprintf(file,"%s %4d: %f\t",str,i,memory[i]);
                printf("%s %4d: %f\t",str,i,memory[i]);
                if((i+1)%8==0){
                        fprintf(file,"\n");
                        printf("\n");
                }
        }

        fprintf(file,"\n\n\n");
        printf("\n\n\n");

}



void display_save_double(FILE * file, double * memory, char * str, int size,int option){
	if(option==0){
		fprintf(file,"Initial %s Values: . . .\n",str);
		printf("Initial %s Values: . . . \n",str);
	}
	else{
		fprintf(file,"Final %s Values: . . .\n",str);
		printf("Final %s Values: . . . \n",str);
	}
	int i;
	for(i=0; i<size; i++){
		fprintf(file,"%s %4d: %lf\t",str,i,memory[i]);
		printf("%s %4d: %lf\t",str,i,memory[i]);
		if((i+1)%8==0){
			fprintf(file,"\n");
			printf("\n");
		}
	}

	fprintf(file,"\n\n\n");
	printf("\n\n\n");

}
