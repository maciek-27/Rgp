FIGUERS=graphics/PurposeOfLibrary.pdf graphics/IdeaOfMUD.pdf\
	graphics/ModuleLayout.pdf graphics/classScr_1_1RScreen__coll__graph.pdf\
	graphics/classScr_1_1RScreen__inherit__graph.pdf \
	graphics/MultiThread.pdf graphics/Nuvola_apps_important.pdf \
	graphics/ChatProgram.pdf

%.eps: %.dia
	dia $< -e $@

%.pdf: %.eps
	epstopdf $<

%.pdf: %.svg
	inkscape $< -A $@


all:$(FIGUERS)
	pdflatex rexio-handbook.tex
	makeindex rexio-handbook.idx
	pdflatex rexio-handbook.tex

clean: 
	rm -f rexio-handbook*