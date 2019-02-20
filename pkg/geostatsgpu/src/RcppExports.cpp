// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// cpp_cholGpu
SEXP cpp_cholGpu(Rcpp::S4 xR, Rcpp::S4 DR, Rcpp::S4 diagWorkingR, Rcpp::S4 diagTimesRowOfAR, int MCglobal, int MClocal, int localStorage, int colGroupwise, int Ncrossprod, int verbose, std::string kernelR);
RcppExport SEXP _geostatsgpu_cpp_cholGpu(SEXP xRSEXP, SEXP DRSEXP, SEXP diagWorkingRSEXP, SEXP diagTimesRowOfARSEXP, SEXP MCglobalSEXP, SEXP MClocalSEXP, SEXP localStorageSEXP, SEXP colGroupwiseSEXP, SEXP NcrossprodSEXP, SEXP verboseSEXP, SEXP kernelRSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::S4 >::type xR(xRSEXP);
    Rcpp::traits::input_parameter< Rcpp::S4 >::type DR(DRSEXP);
    Rcpp::traits::input_parameter< Rcpp::S4 >::type diagWorkingR(diagWorkingRSEXP);
    Rcpp::traits::input_parameter< Rcpp::S4 >::type diagTimesRowOfAR(diagTimesRowOfARSEXP);
    Rcpp::traits::input_parameter< int >::type MCglobal(MCglobalSEXP);
    Rcpp::traits::input_parameter< int >::type MClocal(MClocalSEXP);
    Rcpp::traits::input_parameter< int >::type localStorage(localStorageSEXP);
    Rcpp::traits::input_parameter< int >::type colGroupwise(colGroupwiseSEXP);
    Rcpp::traits::input_parameter< int >::type Ncrossprod(NcrossprodSEXP);
    Rcpp::traits::input_parameter< int >::type verbose(verboseSEXP);
    Rcpp::traits::input_parameter< std::string >::type kernelR(kernelRSEXP);
    rcpp_result_gen = Rcpp::wrap(cpp_cholGpu(xR, DR, diagWorkingR, diagTimesRowOfAR, MCglobal, MClocal, localStorage, colGroupwise, Ncrossprod, verbose, kernelR));
    return rcpp_result_gen;
END_RCPP
}
// gpuNlocal
Rcpp::List gpuNlocal(std::string kernel, std::string functionName, int ctx_id);
RcppExport SEXP _geostatsgpu_gpuNlocal(SEXP kernelSEXP, SEXP functionNameSEXP, SEXP ctx_idSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type kernel(kernelSEXP);
    Rcpp::traits::input_parameter< std::string >::type functionName(functionNameSEXP);
    Rcpp::traits::input_parameter< int >::type ctx_id(ctx_idSEXP);
    rcpp_result_gen = Rcpp::wrap(gpuNlocal(kernel, functionName, ctx_id));
    return rcpp_result_gen;
END_RCPP
}
// cpp_maternGpu
SEXP cpp_maternGpu(SEXP varR, SEXP DofLDLR, SEXP XYR, SEXP crossprodR, SEXP coordsR, SEXP paramR, const int type, const int upper, int max_local_size, const int ctx_id);
RcppExport SEXP _geostatsgpu_cpp_maternGpu(SEXP varRSEXP, SEXP DofLDLRSEXP, SEXP XYRSEXP, SEXP crossprodRSEXP, SEXP coordsRSEXP, SEXP paramRSEXP, SEXP typeSEXP, SEXP upperSEXP, SEXP max_local_sizeSEXP, SEXP ctx_idSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type varR(varRSEXP);
    Rcpp::traits::input_parameter< SEXP >::type DofLDLR(DofLDLRSEXP);
    Rcpp::traits::input_parameter< SEXP >::type XYR(XYRSEXP);
    Rcpp::traits::input_parameter< SEXP >::type crossprodR(crossprodRSEXP);
    Rcpp::traits::input_parameter< SEXP >::type coordsR(coordsRSEXP);
    Rcpp::traits::input_parameter< SEXP >::type paramR(paramRSEXP);
    Rcpp::traits::input_parameter< const int >::type type(typeSEXP);
    Rcpp::traits::input_parameter< const int >::type upper(upperSEXP);
    Rcpp::traits::input_parameter< int >::type max_local_size(max_local_sizeSEXP);
    Rcpp::traits::input_parameter< const int >::type ctx_id(ctx_idSEXP);
    rcpp_result_gen = Rcpp::wrap(cpp_maternGpu(varR, DofLDLR, XYR, crossprodR, coordsR, paramR, type, upper, max_local_size, ctx_id));
    return rcpp_result_gen;
END_RCPP
}
// cpp_lu
SEXP cpp_lu(SEXP xR, const int ctx_id);
RcppExport SEXP _geostatsgpu_cpp_lu(SEXP xRSEXP, SEXP ctx_idSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type xR(xRSEXP);
    Rcpp::traits::input_parameter< const int >::type ctx_id(ctx_idSEXP);
    rcpp_result_gen = Rcpp::wrap(cpp_lu(xR, ctx_id));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_geostatsgpu_cpp_cholGpu", (DL_FUNC) &_geostatsgpu_cpp_cholGpu, 11},
    {"_geostatsgpu_gpuNlocal", (DL_FUNC) &_geostatsgpu_gpuNlocal, 3},
    {"_geostatsgpu_cpp_maternGpu", (DL_FUNC) &_geostatsgpu_cpp_maternGpu, 10},
    {"_geostatsgpu_cpp_lu", (DL_FUNC) &_geostatsgpu_cpp_lu, 2},
    {NULL, NULL, 0}
};

RcppExport void R_init_geostatsgpu(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
