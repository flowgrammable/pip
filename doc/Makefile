
LATEX=pdflatex
BIBTEX=bibtex

tgt=paper

all: refresh

refresh:
	${LATEX} ${tgt}

rebuild:
	${LATEX} ${tgt}
	${BIBTEX} ${tgt}
	${LATEX} ${tgt}
	${LATEX} ${tgt}

bib:
	${BIBTEX} ${tgt}

clean:
	@rm -f ${tgt}.pdf \
				 ${tgt}.out \
				 ${tgt}.aux \
				 ${tgt}.bbl \
				 ${tgt}.blg \
	 			 ${tgt}.pdf \
	 			 ${tgt}.bcf \
	 			 ${tgt}.run.xml \
	 			 *.log \
	 			 texput.log
