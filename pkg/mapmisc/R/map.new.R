map.new = function(x, legendRight=FALSE, buffer=0, mar=c(0,0,0,0), ...) {
	
	if(!is.null(attributes(x)$ellipse)) {
		ellipse = x = attributes(x)$ellipse
	} else {
		ellipse = NULL
	}
	
  xpoints = as(extend(extent(x), buffer), 'SpatialPoints')

	thecrs = try(proj4string(x), silent=TRUE) 
	if(class(thecrs)!="try-error" & !is.na(thecrs)) {
			proj4string(xpoints) = CRS(thecrs)
	}
  
  oldpar = par(c('mar','xaxs','yaxs', 'bty'))

  if(is.logical(legendRight))
    legendRight = c(1,0.8)[1+ legendRight ]
  
  if(!is.na(legendRight))
    mar[4] = (1-legendRight) * par('fin')[1]/par('csi')

	ldots = list(...)
	ldots$mar = mar
	if(!length(ldots$bty) & mar[4]>0 ) ldots$bty='l'
	if(!length(ldots$xpd)) ldots$xpd=FALSE
	if(!length(ldots$axes)) ldots$axes=TRUE
	if(!length(ldots$xaxs)) ldots$xaxs = 'i'
	if(!length(ldots$yaxs)) ldots$yaxs = 'i'
	
  	do.call(par, ldots[setdiff(names(ldots),'axes')]) #(mar=mar, xaxs='i',yaxs='i',xpd=FALSE, ...)
  
	do.call(plot, c(list(x=xpoints,pch=NA), ldots))#, axes=TRUE, ...)

	if(!is.null(ellipse)) {
		plot(ellipse, add=TRUE, ...)
	}
  

  return(invisible(oldpar))
		
}
