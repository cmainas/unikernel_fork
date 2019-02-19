CHAPTERS = abstract.tex chapter1.tex chapter2.tex chapter3.tex chapter4.tex chapter5.tex
OTHERS = ref.bib

diploma.pdf: diploma.tex $(CHAPTERS) $(OTHERS)
	xelatex diploma.tex
	bibtex diploma
	xelatex diploma.tex
	xelatex diploma.tex

clean: 
	rm diploma.pdf

dist-clean: clean
	rm diploma.{log,aux}
