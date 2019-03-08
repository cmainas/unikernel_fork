##########################
## Makefile
##########################

LATEX ?= xelatex
BIBTEX ?= bibtex
DVIPS ?= dvips
PS2PDF ?= ps2pdf
PDFVIEWER ?= acroread
SED ?= /bin/sed

all: presentation.pdf # paper-for-pdf.ps 

clean:
	rm -f *log *aux *blg *bbl *dvi presentation*pdf *ps *~ *toc *nav *out *snm

presentation.dvi: presentation.tex 
	$(LATEX) presentation.tex
	$(LATEX) presentation.tex

presentation.ps: presentation.dvi
	$(DVIPS) presentation.dvi

presentation.pdf: presentation.tex
	#$(PS2PDF) presentation.ps
	$(LATEX) presentation.tex
	$(LATEX) presentation.tex

paper.pdfview: paper.pdf
	"$(PDFVIEWER)" paper.pdf

