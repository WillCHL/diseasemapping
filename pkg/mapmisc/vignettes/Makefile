all: north.html mapmisc.pdf 

clean:
	rm -rf *.md *.tex *.toc *.bbl *.blg *.aux *.out *.log *.spl *tikzDictionary *.md figure/ cache/ *.dvi *.4* *.css *.idv *.lg *.tmp *.xref *.gz my*.bib *.nav *.xml *.vrb *.snm 

empty:
	rm *.pdf *.html

%.pdf: %.tex
	xelatex -interaction=nonstopmode $<;
	xelatex -interaction=nonstopmode $<
	R --slave -e "tools::compactPDF('$@', gs_quality='ebook')" 

%.md: %.Rmd
	R --slave -e "knitr::knit('$<')" fact=2

%.tex: %.Rnw
	R --slave -e "knitr::knit('$<')" fact=2

	
%.html: %.md
	pandoc --smart --self-contained --to=html5 --css=http://diseasemapping.r-forge.r-project.org/webpage.css --output=$@ $<	
