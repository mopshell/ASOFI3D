/*
 * Create the function
 * put it in fd.h
 * use in sofi3D.c
 *
 *
 * Mahesh Kalita 1st December, 2016
 *
 */

#include "fd.h"
//#include <string.h>




void removespace(char *str) {

    char *p1 = str, *p2 = str;
    do
	while (*p2 == ' ' || *p2 == '\t')
	    p2++;
    while ((*p1++ = *p2++));
}



void madinput(char header[], float *** DEN ){


    // -------------  header file Reading ---------------------
    extern float DX, DY, DZ;//, OX, OY, OZ;
    extern int NX, NY, NZ, NXG, NYG, NZG, POS[4], MYID;
    extern FILE *FP;
    fprintf(FP,"\n\n\n--------------------------------------------------------- \n");
    fprintf(FP," \n \n *********** Madagascar Input Start *************** \n \n");
    fprintf(FP,"--------------------------------------------------------- \n");
    fprintf(FP,"\n...Madagascar file : \t%s \n",header);
    fflush( FP);
    // local variables
    int i, ii, j, jj, k, kk;

    int Nx, Nz, Ny;
    float Dx, Dz, Dy;
    float Ox, Oy,Oz;
    float tempRho=0.0;

    // -------------  header file Reading ---------------------
    char *pch;
    FILE *ioh_file;
    char filename[STRING_SIZE];
    char *s="\"";
    char *token;
    char binary_file[STRING_SIZE];

    char tempbuff[STRING_SIZE];

/*
    // Read the header file
    sprintf(filename,"%s",header);
    ioh_file=fopen(filename,"r");
    if (ioh_file==NULL) err("\t \t \t :( Could not open Header :( ");

    while(!feof(ioh_file)){
	if(fgets(tempbuff,STRING_SIZE,ioh_file)!=NULL ){
	    pch = strtok (tempbuff," =");
	    removespace(tempbuff);

	    if (strcmp(pch,"in")==0)    {

		pch = strtok (NULL, " =");
		pch[strcspn(pch,"\r\n")]=0; // to remove the newline character
		token = strtok(pch,s); // remove the ""
		strcpy(binary_file,token);
	    }
	    else if (strcmp(pch,"n1")==0)       {
		pch = strtok (NULL, " = ");
		Nz=atoi(pch);
	    }
	    else if (strcmp(pch,"n2")==0)       {
		pch = strtok (NULL, " = ");
		Nx=atoi(pch);
	    }
	    else if (strcmp(pch,"n3")==0)       {
		pch = strtok (NULL, " = ");
		Ny=atoi(pch);
	    }
	    else if (strcmp(pch,"d1")==0)       {
		pch = strtok (NULL, " = ");
		Dz=atof(pch);
	    }

	    else if (strcmp(pch,"d2")==0)       {
		pch = strtok (NULL, " = ");
		Dx=atof(pch);
	    }

	    else if (strcmp(pch,"d3")==0)       {
		pch = strtok (NULL, " = ");
		Dy=atof(pch);
	    }
	    else if (strcmp(pch,"o1")==0)       {
		pch = strtok (NULL, " = ");
		Oz=atof(pch);
	    }
	    else if (strcmp(pch,"o2")==0)       {
		pch = strtok (NULL, " = ");
		Ox=atof(pch);
	    }

	    else if (strcmp(pch,"o3")==0)       {
		pch = strtok (NULL, " = ");
		Oy=atof(pch);
	    }

	}
    }

    // over-write the json parameters
    //NXG=Nx;
    //NZG=Nz;
    //NYG=Ny;
    //DZ=Dz;
    //DX=Dx;
    //DY=Dy;
    // ----------------------- Header informaition Reading Completed ---------------




    // --------- Binary allocation to Memory ---------------------------

    const int format=3;

    ioh_file=fopen(binary_file,"r");
    fprintf(FP,"\n\n..binary \t%s\n\n",binary_file);
    if (ioh_file==NULL) err("\t \t \t :( No binaries  present :( ");
*/

    fprintf(FP,"MYID=%d\n\n",MYID);
    for (k=1;k<=NZG;k++){
      for (i=1;i<=NXG;i++){
        for (j=1;j<=NYG;j++){
          //tempRho=readdsk(ioh_file, format);
          if ((POS[1]==((i-1)/NX)) && (POS[2]==((j-1)/NY)) && (POS[3]==((k-1)/NZ))){

            ii=i-POS[1]*NX;
            jj=j-POS[2]*NY;
            kk=k-POS[3]*NZ;
            DEN[jj][ii][kk]=15000;
          }
        }
      }
    }

//    fclose(ioh_file);
    fprintf(FP,"\n\n\n--------------------------------------------------------- \n");
    fprintf(FP," \n \n *********** Madagascar Input Finish *************** \n \n");
    fprintf(FP,"--------------------------------------------------------- \n");
}
