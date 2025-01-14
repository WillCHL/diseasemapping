#' @title fisher_sim gpu
#'
#' @description Fisher simulation test
#'
#' @param sr vector
#' @param sc vector
#' @param x vector
#' @param workgroupSize workgroup size
#' @param streams vector of integers, 18x workgroupSize
#' @return altered in-place
#' @useDynLib clRNG
#' @export

fisher_simGpu=function(
  sr, ##marginal row total 
  sc, ##marginal column total
  x,## the vector to stall test statistics
  streams, 
  workgroupSize){
  
  verbose = TRUE
  
  
  if(missing(streams)) {
    if(missing(workgroupSize)) 
    {workgroupSize = c(64,4)}
    workgroupSize = pmax(4, c(workgroupSize, 4, 4)[1:2])
    streams = cpp_mrg31k3pCreateStreams(prod(workgroupSize))			
  } else {
    if(!is.matrix(streams)) {
      warning("streams should be a matrix")
    }
    if(ncol(streams) != 18) {
      warning("streams needs 18 columns")
    }
    if(missing(workgroupSize)) {
      workgroupSize = c(nrow(streams)/2,2)
    }
    if(prod(workgroupSize) != nrow(streams))
      warning("number of work items needs to be same as number of streams")
    # make a deep copy
    streams = matrix(as.vector(streams), nrow(streams), ncol(streams), FALSE, dimnames(streams))
     }
  
  if(verbose) {
    cat('\nglobal sizes ', toString(workgroupSize), '\n streams ', toString(dim(streams)), '\n')
  }
   #sr<- as.integer(sr)
   #sc<- as.integer(sc)
  
  cpp_fisher_sim_gpu(sr, sc, x, streams, workgroupSize)
  
  invisible(streams)
  
}



















