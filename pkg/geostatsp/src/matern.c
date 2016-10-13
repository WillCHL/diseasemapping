/* type = 0, distances is a vector
 type = 1, distances is a symmetric matrix,
 only lower triangle is used
 type = 2, as type=1 but return the cholesky
 type = 3, as type=1, but return inverse
 nugget is only used if type >1
 and added to the diagonals

If the cholkesy is performed (type > 1):
 halfLogDet is the log determinant if the cholesky matrix
 type is info from dpotrf
if the precision is computed type is info from dpotrfi
... and if chol of precision is computed, type is from dtrtri
 */

#include"geostatsp.h"


void maternArasterBpoints(
		double *Axmin, double *Axres,
		int *AxN,
		double *Aymax, double *Ayres, int *AyN,
		double *Bx, double *By, int *BN,
		double *result,
		double  *range, double*shape, double *variance,
		double *anisoRatio, double *anisoAngleRadians) {

	int DB, DAx, DAy, AyN2, AxN2, BN2;
	int Dindex,Ncell;
	double distCellRight[2], distCellDown[2], distTopLeft[2], distRowHead[2];
	double distTopLeftR[2], distHere[2];
	double costheta, sintheta, anisoRatioSq;
	double logxscale, xscale, varscale,  thisx, logthisx;
	int nb;
	double *bk, alpha,truncate;


	AyN2 = *AyN;
	AxN2 = *AxN;
	Ncell = AyN2*AxN2;
	BN2 = *BN;
	*Axmin += *Axres/2; // add half a cells size, assign each cell
	*Aymax -= *Ayres/2; // it's centroid rather than it's top left corner


	costheta = cos(*anisoAngleRadians);
	sintheta = sin(*anisoAngleRadians);
	anisoRatioSq = (*anisoRatio)*(*anisoRatio);

	distCellRight[0] = costheta *(*Axres);
	distCellRight[1] = sintheta * (*Axres);

	distCellDown[0] =  sintheta * (*Ayres);
	distCellDown[1] =  - costheta * (*Ayres);

	xscale = 2 * M_SQRT2 * sqrt( *shape ) / *range;
	logxscale  =  1.5 * M_LN2 +   0.5 * log(*shape)  - log(*range);
	varscale =  log(*variance)  - lgammafn(*shape ) -  (*shape -1)*M_LN2;

	truncate = *variance*1e-06; // count a zero if var < truncate

	alpha = *shape;
	// code stolen from R's src/nmath/bessel_k.c
	nb = 1+ (int)floor(alpha);/* nb-1 <= |alpha| < nb */
	bk = (double *) calloc(nb, sizeof(double));


	//Nzeros=0;
	//#pragma omp parallel for private(distTopLeft,distTopLeftR,distRowHead,DAy,distHere,thisx,Dindex)
	for(DB=0;DB<BN2;++DB){ // loop through points
		Dindex = DB*Ncell;
		distTopLeft[0]= (Bx[DB]-*Axmin); // distance from point DB to
		distTopLeft[1]= (By[DB]-*Aymax); //   top left corner of raster

		distTopLeftR[0]= costheta * distTopLeft[0] - sintheta * distTopLeft[1];
		distTopLeftR[1] = sintheta *distTopLeft[0] + costheta * distTopLeft[1];

		distRowHead[0] = distTopLeftR[0]; // distance to leftmost cell of row DAy
		distRowHead[1] = distTopLeftR[1];

		for(DAy=0;DAy<AyN2;++DAy){ // loop through y of raster

			distHere[0] = distRowHead[0]; // dist to cell DAx Day
			distHere[1] = distRowHead[1];
			for(DAx=0;DAx<AxN2;++DAx){ // loop through x of raster

				//			thisx =  sqrt(distHere[0]*distHere[0] +
				//	      			distHere[1]*distHere[1]/anisoRatioSq)*xscale;

				thisx = distHere[0]*distHere[0] +
						distHere[1]*distHere[1]/anisoRatioSq;
				logthisx = 0.5*log(thisx) + logxscale;
				thisx =sqrt(thisx) * xscale;


				// if thiex is nan assume it's infinity
				if(isnan(thisx)) {
					if(isinf(xscale)) {
						// range is probably zero.
						// if distance is zero set result to variance
						if(distHere[0]*distHere[0] +
								distHere[1]*distHere[1] < truncate){
							result[Dindex]= *variance;
						}
					} else {
						// range is finite, distance must be zero
						result[Dindex] = 0;
					}
				} else {
					result[Dindex] = exp(varscale + *shape * logthisx)*
							bessel_k_ex(thisx, alpha, 1.0, bk);
				}

				if(isnan(result[Dindex]))  {
					// assume distance is very small
					if(thisx < 1) {
						result[Dindex]= *variance;
					} else {
						result[Dindex]= 0;
					}
				}


				//    		if(result[Dindex]  <  truncate) ++Nzeros;

				++Dindex;
				distHere[0] -= distCellRight[0];
				distHere[1] -= distCellRight[1];
			}

			distRowHead[0] -=distCellDown[0];
			distRowHead[1] -=distCellDown[1];

		}

	}
	//*BN = Nzeros;
	//*range = xscale;
	//*shape=varscale;
	//*anisoRatio = anisoRatioSq;
	free(bk);

}

// returns an N by N matrix for matern correlation
// for N points with vectors of coordinates x and y
// type = 0 or 1 return correlation,
// type=2 chol, type=3 precision, type 4 chol of precsion
void maternAniso(
		const double *x,
		const double *y,
		const int *N,
		double *result,
		const double *range,
		const double *shape,
		const double *variance,
		const double *anisoRatio,
		const double *anisoAngleRadians,
		const double *nugget,
		int *type,
		double *halfLogDet
) {
	// type=2 return cholesky
	int Drow, Dcol, Nm1, Dcolp1, N2;
	int Dindex;

	double logxscale, varscale,  logthisx, thisx;
	double anisoRatioSq, dist[2], distRotate[2], costheta, sintheta;

	int nb;
	double *bk, alpha,truncate=0.00001;

	costheta = cos(*anisoAngleRadians);
	sintheta = sin(*anisoAngleRadians);
	anisoRatioSq = (*anisoRatio)*(*anisoRatio);

	//	xscale = sqrt(8 * (*shape)) / *range;
	//	logxscale  = 0.5*(log(8) + log(*shape) ) - log(*range);
	logxscale  =  1.5 * M_LN2 +   0.5 * log(*shape)  - log(*range);
	varscale =  log(*variance)  - lgammafn(*shape ) -  (*shape -1)*M_LN2;

	alpha = *shape;
	// code stolen from R's src/nmath/bessel_k.c
	nb = 1+ (int)floor(alpha);/* nb-1 <= |alpha| < nb */
	bk = (double *) calloc(nb, sizeof(double));

	Nm1 = *N-1;
	N2 = *N;

	result[N2*N2-1] = *variance + *nugget;
	for(Dcol=0;Dcol < Nm1;++Dcol) {
		Dcolp1 = Dcol + 1;
		Dindex = Dcol*N2+Dcol;
		result[Dindex] = *variance + *nugget;
		for(Drow=Dcolp1; Drow < N2; ++Drow) {
			Dindex++;
			dist[0] = x[Dcol] - x[Drow];
			dist[1] = y[Dcol] - y[Drow];

			// rotate anticlockwise by aniso.angle.radians
			// distRotate =  ( cos(theta)  -sin(theta)  )  dist
			//               ( sin(theta)   cos(theta)  )
			distRotate[0] = costheta *dist[0] - sintheta * dist[1];
			distRotate[1] = sintheta *dist[0] + costheta * dist[1];

			//    		thisx =  sqrt(distRotate[0]*distRotate[0] +
			//    			distRotate[1]*distRotate[1]/anisoRatioSq)*xscale;

			thisx = distRotate[0]*distRotate[0] +
					distRotate[1]*distRotate[1]/anisoRatioSq;
			logthisx = 0.5*log(thisx) + logxscale;
			thisx =exp(logthisx);

			if(isnan(thisx)) {
				if(isinf(logxscale)) {
					// range is probably zero.
					// if distance is zero set result to variance
					if(distRotate[0]*distRotate[0] +
							distRotate[1]*distRotate[1] < truncate){
						result[Dindex]= *variance;
					}
				} else {
					// range is finite, distance must be zero
					result[Dindex] = 0;
				}// end else from isinf logscale
			} else { // thisx not nan
				result[Dindex] = exp(varscale + *shape * logthisx )*
						bessel_k_ex(thisx, alpha, 1.0, bk);
			}// end else thisx not nan

			if(isnan(result[Dindex]))  {
				// assume distance is very small
				if(thisx < 1) {
					result[Dindex]= *variance;
				} else {
					result[Dindex]= 0;
				}
			} // end if isnan
		} // end for Drow
	}// end for Dcol


	if(*type >1 ){ // cholesky
		F77_CALL(dpotrf)("L", N, result, N, &Dcol);
		*halfLogDet=0;  // the log determinant
		for(Drow = 0; Drow < N2; Drow++)
			*halfLogDet += log(result[Drow*N2+Drow]);
		if(*type == 3){ // precision
			F77_NAME(dpotri)("L", N,
					result, N,&Dcol);
		} else if (*type==4) {// cholkesy of precision
			F77_NAME(dtrtri)("L", "N",N,
					result, N,&Dcol);
		} else {
			Drow = 0;
		}
		*type = Dcol;
	}

	free(bk);
}

void matern(
		const double *distance,
		const int *N,
		double *result,
		const double *range,
		const double *shape,
		const double *variance,
		const double *nugget,
		int *type,
		double *halfLogDet) {
	int D, Dcol, Ncol, Nrow, rowEnd, addToRowStart;
	double varscale,  thisx, //xscale,
	logthisx, logxscale;

	int nb;
	double *bk, alpha;

	alpha = *shape;

	// code stolen from R's src/nmath/bessel_k.c
	nb = 1+ (int)floor(alpha);/* nb-1 <= |alpha| < nb */
	bk = (double *) calloc(nb, sizeof(double));

	// evaluate the matern!
	/*
	thisx = abs(x)*(sqrt(8*param["shape"])/ param["range"])
	result = ( param["variance"]/(gamma(param["shape"])* 2^(param["shape"]-1)  ) ) *
			( thisx^param["shape"] *
				besselK(thisx , param["shape"]) )
	 */
	// xscale = sqrt(8*shape)/range
	// thisx = xscale * abs(x)
	// varscale = log[   param["variance"]/(gamma(param["shape"])* 2^(param["shape"]-1)  )   ]
	// result = exp(varscale) * (thisx)^nu K(thisx, nu)
	// result = exp(varscale + nu * log(thisx)) K(thisx, nu)
	// result = exp(varscale + nu * logthisx) K(thisx, nu)
	//	xscale = 2 * M_SQRT2 * sqrt( *shape ) / *range;
	logxscale  =  1.5 * M_LN2 +   0.5 * log(*shape)  - log(*range);
	varscale =  log(*variance)  - lgammafn(*shape ) -  (*shape -1)*M_LN2;

	Nrow = *N;
	if(*type){ // lower triangle
		for(Dcol=0;Dcol<Nrow;++Dcol){
			// diagonals
			result[Dcol*Nrow+Dcol] =
					*variance + *nugget;
		}
		Ncol = *N-1; // the last column isn't done
		addToRowStart = 1; // the diagonals aren't done with materns
	} else {
		Ncol=1;
		addToRowStart=0;
	}

	for(Dcol=0;Dcol<Ncol;++Dcol) {
		rowEnd = Nrow*Dcol+Nrow;
		for(D=Dcol*Nrow+Dcol+addToRowStart; D < rowEnd; D++) {
			//		thisx = fabs(distance[D])*xscale;
			logthisx = log(fabs(distance[D])) + logxscale;
			thisx = exp(logthisx);

			if(isnan(thisx) ) {
				//			warning("%f %f", thisx, xscale);
				if(isinf(logxscale)) {
					// range is probably zero.
					result[D] = 0;
				} else {
					result[D] = *variance;
				}
			} else { // thisx not nan
				result[D] = exp(varscale + *shape * logthisx )*
						bessel_k_ex(thisx, alpha, 1.0, bk);
			}
			if(isnan(result[D])) {
				// assume distance is very small
				if(thisx < 1) {
					result[D]= *variance;
				} else {
					result[D]= 0;
				}
			}
		} //D
	} // Dcol

	if(*type >1 ){ // cholesky
		F77_CALL(dpotrf)("L", N, result, N, &Dcol);
		*halfLogDet=0;  // the log determinant
		for(D = 0; D < Nrow; D++)
			*halfLogDet += log(result[D*Nrow+D]);
		if(*type == 3){//precision
			F77_NAME(dpotri)("L", N,
					result, N,&Dcol);
		} else if (*type==4) {// cholesky of precision
			F77_NAME(dtrtri)("L", "N",N,
					result, N,&Dcol);
		} else {
			D = 0;
		}
		*type = Dcol;
	}
	free(bk);
}

// matern for a vector of parameters
void maternForL(
		const double *xcoord,
		const double *ycoord,
		const int *N,
		double *corMat,
		const double *param,
		// nugget, variance,
		// range, shape,
		// anisoRatio, ansioAngleRadians
		const int *aniso,
		const int *withoutNugget,
		int *type,
		double *halfLogDet
){


	double nugget;

	if(*withoutNugget){
		nugget = 0.0;
	} else {
		nugget = param[0];
	}

	if(*aniso) {
		maternAniso(
				xcoord,ycoord,
				N,
				corMat,
				&param[2],
				&param[3],
				&param[1],
				&param[4],
				&param[5],
				&nugget,
				type,
				halfLogDet);
	} else {
		matern(xcoord,
				N,
				corMat,
				&param[2],
				&param[3],
				&param[1],
				&nugget,
				type,
				halfLogDet);
	}


}

int typeStringToInt(SEXP type){

	const char *typeSeq[] = {"variance", "cholesky", "precision","inverseCholesky"};
	int D, typeInt;

	// find type integer, and check it's valid
	for(D = 0;D< 4;++D){
		if( strcmp(CHAR(STRING_ELT(type, 0)), typeSeq[D]) == 0 ) {
			typeInt = D;
		}
	}
	if(typeInt>3) {
		Rprintf("d %s %s", CHAR(STRING_ELT(type, 0)), typeSeq[0]);
		error("'type' in matern, must be one of 'variance','cholesky','precision','inverseCholesky'");
	} else {
		// indexed from 1
		typeInt = typeInt + 1;
	}
	return typeInt;
}



SEXP maternPoints(
		SEXP points,
		SEXP param,
		// range,
		// shape,
		// variance,
		// anisoRatio,
		// anisoAngleRadians,
		// nugget,
		SEXP type) {

	const char *valid[] = {"SpatialPoints", "SpatialPointsDataFrame"};
	SEXP halfLogDet, result, dim, resultX, resultUplo;
	int typeInt, N;
	double *P;

	// check points are SpatialPoints*
	if(strcmp(
			CHAR(STRING_ELT(getAttrib(points, R_ClassSymbol), 0)),
			valid[0]
	) *	strcmp(
			CHAR(STRING_ELT(getAttrib(points, R_ClassSymbol), 0)),
			valid[1]
	) != 0) {
		Rprintf("class %s\n",
				CHAR(STRING_ELT(getAttrib(points, R_ClassSymbol), 0))
		);
		error("invalid class of 'points' in maternPoints, must be SpatialPoints*");
	}

	N = INTEGER(getAttrib(
			GET_SLOT(points, install("coords")),
			R_DimSymbol))[0];
	P = REAL(GET_SLOT(points, install("coords")));

	typeInt = typeStringToInt(type);

	PROTECT(resultX = NEW_NUMERIC(N*N));
	PROTECT(halfLogDet = NEW_NUMERIC(1));
	PROTECT(dim = NEW_INTEGER(2));
	PROTECT(resultUplo = ScalarString(mkChar("L")));

	INTEGER(dim)[0] = N;
	INTEGER(dim)[1] = N;


	if(typeInt == 2 | typeInt == 4) {
		// lower triangle
		PROTECT(result = NEW_OBJECT(MAKE_CLASS("dtrMatrix")));
	} else {
		// symmetric
		PROTECT(result = NEW_OBJECT(MAKE_CLASS("dsyMatrix")));
	}


	SET_SLOT(result, install("x"), resultX);
	SET_SLOT(result, install("Dim"), dim);
	SET_SLOT(result, install("uplo"), resultUplo);
	setAttrib(result, install("type"), duplicate(type));
	setAttrib(result, install("param"), duplicate(param));

	maternAniso(
			P, // x
			&P[N], // y
			&N,
			REAL(resultX),
			&REAL(param)[0],// range,
			&REAL(param)[1],// shape,
			&REAL(param)[2],// variance,
			&REAL(param)[3],// anisoRatio,
			&REAL(param)[4],// anisoAngleRadians,
			&REAL(param)[5],// nugget,
			&typeInt,
			REAL(halfLogDet)
	);

	if(typeInt > 1) {
		setAttrib(result, install("halfLogDet"), halfLogDet);
	}

	UNPROTECT(5);
	return result;
}



SEXP maternDistance(
		SEXP distance,
		SEXP param,
		// range, shape,
		// variance, nugget,
		SEXP type
		//c('variance','cholesky','precision','inverseCholesky')
) {

	SEXP result, halfLogDet;
	const char
	*valid[] = {"dsyMatrix"};
	int typeInt=5, D, D2, N;
	double *P;

	N = INTEGER(GET_SLOT(distance, install("Dim")))[0];
	P = REAL(GET_SLOT(distance, install("x")));
	PROTECT(halfLogDet = NEW_NUMERIC(1));

	// check distance is symmetric
	if(R_check_class_etc(distance, valid)) {
		error("invalid class of 'distance' in maternDistance, must be dsyMatrix");
	}

	// check values stored in lower triangle
	if(strcmp(
			CHAR(STRING_ELT(GET_SLOT(distance, install("uplo")), 0) ),
			"L"
	) != 0) {
		// not lower triangle, copy over
		for (D = 1; D < N; D++) // rows
			for (D2 = 0; D2 < D && D2 < N; D2++) // columns
				P[D + D2*N] = P[D2 + D*N];
	}

	typeInt = typeStringToInt(type);

	if(typeInt == 2 | typeInt == 4) {
		// lower triangle
		PROTECT(result = NEW_OBJECT(MAKE_CLASS("dtrMatrix")));
	} else {
		// symmetric
		PROTECT(result = NEW_OBJECT(MAKE_CLASS("dsyMatrix")));
	}

	SET_SLOT(result, install("x"),
			duplicate(GET_SLOT(distance, install("x"))));
	SET_SLOT(result, install("Dim"),
			duplicate(GET_SLOT(distance, install("Dim"))));
	SET_SLOT(result, install("Dimnames"),
			duplicate(GET_SLOT(distance, install("Dimnames"))));
	SET_SLOT(result, install("uplo"),
			ScalarString(mkChar("L")));

	setAttrib(result, install("type"), duplicate(type));
	setAttrib(result, install("param"), duplicate(param));


	matern(
			P,
			&N, //N
			REAL(GET_SLOT(result, install("x"))),
			&REAL(param)[0],// range,
			&REAL(param)[1],// shape,
			&REAL(param)[2],// variance,
			&REAL(param)[3],// nugget,
			&typeInt,
			REAL(halfLogDet));


	if(typeInt > 1) {
		setAttrib(result, install("halfLogDet"), halfLogDet);
	}

	UNPROTECT(2);
	return result;
}

