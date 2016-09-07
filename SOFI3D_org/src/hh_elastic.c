/*------------------------------------------------------------------------
 * Copyright (C) 2011 For the list of authors, see file AUTHORS.
 *
 * This file is part of SOFI3D.
 *
 * SOFI3D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.0 of the License only.
 *
 * SOFI3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SOFI3D. See file COPYING and/or
  * <http://www.gnu.org/licenses/gpl-2.0.html>.
--------------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 *   Generates elastic model properties (vp,vs,density) on the fly
 *
 *   depending on model dimension in vertical direction and local variable "h"
 *   this function can generate a
 *   	-> homogeneneous full space
 *   	-> layer over half space
 *
 *  ----------------------------------------------------------------------*/

#include "fd.h"

void model_elastic(float  ***  rho, float ***  pi, float ***  u,
        float *** C11, float *** C12, float *** C13, float *** C22, float *** C23, float *** C33,
        float *** C44, float *** C55, float *** C66,
		float ***  taus, float ***  taup, float *  eta){

	/*--------------------------------------------------------------------------*/
	/* extern variables */
	extern float DY;
	extern int NX, NY, NZ, NXG, NYG, NZG, POS[4], L, MYID;
	extern char  MFILE[STRING_SIZE];
	extern int WRITE_MODELFILES;

	/* local variables */
	float muv, piv;
	float Vpv, Vsv, Rho, Poi, Epsx, Epsy, Delx, Dely, Delxy, Gamx, Gamy;
	float *** vpv=NULL, *** vsv=NULL, *** epsx=NULL, *** epsy=NULL, *** gamx=NULL;
        float *** delx=NULL, *** dely=NULL, *** delxy=NULL, *** gamy=NULL;
	float y;
	int i, j, k, ii, jj, kk;
	char modfile[STRING_SIZE];

	/*-----------------material property definition -------------------------*/

        /* x=1, y=2 in Tsvankin [1997] (e.g.) epsx=epsion1 & epsy=epsilon2 */

	/* parameters for layer 1 */
	const float vpv1=3500.0, poi1=0.25, epsx1=0.1, epsy1=0.2, delx1=0.1, dely1=0.3, delxy1=0.1,
          gamx1=0.3, gamy1=0.1, rho1=2000.0, h=100000.0;

	/* parameters for layer 2 */
	//const float vp2=5700.0, vs2=3400.0, rho2=2500.0;
	const float vpv2=3500.0, poi2=0.25, epsx2=0., epsy2=0., delx2=0., dely2=0., delxy2=0.,
          gamx2=0., gamy2=0., rho2=2000.0;


	if (WRITE_MODELFILES==1) {
		vpv  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
		vsv  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
		epsx  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
		epsy  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
	        delx  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
		dely  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
		delxy  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
		gamx  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
		gamy  =  f3tensor(0,NY+1,0,NX+1,0,NZ+1);
	}

	/*elastic simulation */
	if (L==0) {
		/* loop over global grid */
		for (k=1;k<=NZG;k++){
			for (i=1;i<=NXG;i++){
				for (j=1;j<=NYG;j++){

					/*=========================================================
					 * modify below this point for ELASTIC model definition
					 *=========================================================
					 */
					/*note that "y" is used for the vertical coordinate*/
					/* calculate vertical coordinate in m */

					y=(float)j*DY;

					/* two layer case */
					if (y<=h){
						Vpv=vpv1; Poi=poi1; Epsx=epsx1; Epsy=epsy1; Delx=delx1; Dely=dely1;
                                                Delxy=delxy1; Gamx=gamx1; Gamy=gamy1; Rho=rho1; }


					else{
						Vpv=vpv2; Poi=poi2; Epsx=epsx2; Epsy=epsy2; Delx=delx2; Dely=dely2;
                                                Delxy=delxy2; Gamx=gamx2; Gamy=gamy2; Rho=rho2; }

					/*=========================================================
					 * modify up to this point for ELASTIC model definition
					 *=========================================================
					 */

                                        Vsv=Vpv*sqrt((1-2*Poi)/(2-2*Poi));
					muv=Vsv*Vsv*Rho;
					piv=Vpv*Vpv*Rho;

					/* only the PE which belongs to the current global gridpoint
							is saving model parameters in his local arrays */
					if ((POS[1]==((i-1)/NX)) &&
							(POS[2]==((j-1)/NY)) &&
							(POS[3]==((k-1)/NZ))){
						ii=i-POS[1]*NX;
						jj=j-POS[2]*NY;
						kk=k-POS[3]*NZ;

						u[jj][ii][kk]=muv;
             					pi[jj][ii][kk]=piv;

	                          		C33[jj][ii][kk]=Rho*Vpv*Vpv;
                                                C55[jj][ii][kk]=Rho*Vsv*Vsv;
                                                C11[jj][ii][kk]=(1+2*Epsx)*Rho*Vpv*Vpv;
                                                C22[jj][ii][kk]=(1+2*Epsy)*Rho*Vpv*Vpv;
                                                C66[jj][ii][kk]=(1+2*Gamx)*Rho*Vsv*Vsv;
                                                C44[jj][ii][kk]=((1+2*Gamx)*Rho*Vsv*Vsv)/(1+2*Gamy);
                                                C13[jj][ii][kk]=Rho*sqrt((Vpv*Vpv-Vsv*Vsv)*((1+2*Delx)*Vpv*Vpv-Vsv*Vsv))-Rho*Vsv*Vsv ;
                                                C23[jj][ii][kk]=Rho*sqrt((Vpv*Vpv-(1+2*Gamx)*Vsv*Vsv/(1+2*Gamy))*((1+2*Dely)*Vpv*Vpv-(1+2*Gamx)*Vsv*Vsv/(1+2*Gamy)))-Rho*(1+2*Gamx)*Vsv*Vsv/(1+2*Gamy);
                                                C12[jj][ii][kk]=Rho*sqrt(((1+2*Epsx)*Vpv*Vpv-(1+2*Gamx)*Vsv*Vsv)*((1+2*Delxy)*(1+2*Epsx)*Vpv*Vpv-(1+2*Gamx)*Vsv*Vsv))-Rho*(1+2*Gamx)*Vsv*Vsv ;
						rho[jj][ii][kk]=Rho;


						if (WRITE_MODELFILES==1) {
							vpv[jj][ii][kk]=Vpv;
							vsv[jj][ii][kk]=Vsv;
						}
					}
				}
			}
		}
	}


	/* each PE writes his model to disk */

	/* all models are written to file */
	if (WRITE_MODELFILES==1) {
		sprintf(modfile,"%s.SOFI3D.pi",MFILE);
		writemod(modfile,pi,3);
		MPI_Barrier(MPI_COMM_WORLD);
		if (MYID==0) mergemod(modfile,3);

		sprintf(modfile,"%s.SOFI3D.u",MFILE);
		writemod(modfile,u,3);
		MPI_Barrier(MPI_COMM_WORLD);
		if (MYID==0) mergemod(modfile,3);

		sprintf(modfile,"%s.SOFI3D.vp",MFILE);
		writemod(modfile,vpv,3);
		MPI_Barrier(MPI_COMM_WORLD);
		if (MYID==0) mergemod(modfile,3);

		sprintf(modfile,"%s.SOFI3D.vs",MFILE);
		writemod(modfile,vsv,3);
		MPI_Barrier(MPI_COMM_WORLD);
		if (MYID==0) mergemod(modfile,3);

		sprintf(modfile,"%s.SOFI3D.rho",MFILE);
		writemod(modfile,rho,3);
		MPI_Barrier(MPI_COMM_WORLD);
		if (MYID==0) mergemod(modfile,3);
	}

	/* only density is written to file */
	if (WRITE_MODELFILES==2) {
		sprintf(modfile,"%s.SOFI3D.rho",MFILE);
		writemod(modfile,rho,3);
		MPI_Barrier(MPI_COMM_WORLD);
		if (MYID==0) mergemod(modfile,3);
	}


	if (WRITE_MODELFILES==1) {
		free_f3tensor(vpv,0,NY+1,0,NX+1,0,NZ+1);
		free_f3tensor(vsv,0,NY+1,0,NX+1,0,NZ+1);
		free_f3tensor(epsx,0,NY+1,0,NX+1,0,NZ+1);
		free_f3tensor(epsy,0,NY+1,0,NX+1,0,NZ+1);
		free_f3tensor(delx,0,NY+1,0,NX+1,0,NZ+1);
		free_f3tensor(dely,0,NY+1,0,NX+1,0,NZ+1);
		free_f3tensor(delxy,0,NY+1,0,NX+1,0,NZ+1);
		free_f3tensor(gamx,0,NY+1,0,NX+1,0,NZ+1);
		free_f3tensor(gamy,0,NY+1,0,NX+1,0,NZ+1);
	}

}
