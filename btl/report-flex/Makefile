.PHONY: all
all: report.pdf

report.pdf: report.bbl 	\
			report.tex 	\
			sections/title-page.tex \
			sections/introduction.tex \
			sections/input-file-structure.tex \
			sections/start-conditions.tex \
			sections/using-flex.tex \
			sections/input-file-for-PLO.tex
	pdflatex report.tex

report.bbl: report.bib 
	pdflatex report.tex
	bibtex report
	pdflatex report.tex
