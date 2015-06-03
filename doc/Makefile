FRONTPAGE=doc/frontpage.svg
SHORTNAME=rexio
LONGNAME=RexIO Terminal Control Library 1.0
TMPLATEX=latex/tmpfrontpage.tex
HANDBOOK=handbook/$(SHORTNAME)-handbook.tex


HEADERS= ../include/rexio/*.h++ ../include/rexio/tk/*.h++ ../lib/*/include/*.h++
SOURCETREE = $(HEADERS) ../lib/*/src/*.c++ ../lib/*/src/*/*.c++
all:  $(SHORTNAME)-refmanual.pdf $(SHORTNAME)-listing.pdf $(SHORTNAME)-handbook.pdf

clean: 
	@echo "[I] Cleaning documentation"
	@rm -rf latex
	@echo "[I] Cleaning printable source code listing"
	@rm -rf listing
	@rm -f frontpage.pdf
	@echo "[I] Cleaning handbook"
	@make -C handbook clean

frontpage.pdf: frontpage.svg
	 inkscape frontpage.svg -A frontpage.pdf


latex/refman.tex: $(HEADERS)
	@echo "[I] generating reference manual"
	@cd .. ; cp Doxyfile Doxyfile_work; \
	sed -i 's/%ShortName%/$(SHORTNAME)/g' Doxyfile_work;\
	doxygen Doxyfile_work;\
	rm -f Doxyfile_work

latex/refman.pdf: latex/refman.tex
	@echo "[I] Compiling to PDF"
	@make -C latex > latex/latex_log

DOCFRONTPAGE: frontpage.tex frontpage.pdf
	@echo "[I] Creating  front page"
	@cat frontpage.tex > $(TMPLATEX)
	@echo \\end{document} >> $(TMPLATEX)
	@sed -i 's/%ProjectName%/$(LONGNAME)/g' $(TMPLATEX)
	@sed -i 's/%DocTitle%/Library reference manual/g' $(TMPLATEX)
	@sed -i "s/%RevNum%/`svn info  | grep Revision | tr -d "[:alpha:]: "`/g" \
	$(TMPLATEX)
	@echo "[I] Compiling front page to PDF"
	@cd latex; pdflatex ../$(TMPLATEX) 

$(SHORTNAME)-refmanual.pdf: latex/refman.pdf 
	make DOCFRONTPAGE
	@echo "[I] adding frontpage to file"
	@pdfjoin latex/tmpfrontpage.pdf latex/refman.pdf --outfile $(SHORTNAME)-refmanual.pdf
	@echo "[I] Reference manual ready"


$(SHORTNAME)-listing.pdf: frontpage.pdf $(SOURCETREE) genlisting.sh
	@echo "[I] Preparing printable source code listing"
	@if [ ! -e listing ] ; then mkdir listing ; fi
	@./genlisting.sh .. > listing/listing.tex
	@sed -i 's/%ProjectName%/$(LONGNAME)/g' listing/listing.tex
	@sed -i 's/%DocTitle%/Comprehensive source code listing/g'\
			listing/listing.tex
	@sed -i "s/%RevNum%/`svn info  | grep Revision | tr -d "[:alpha:]: "`/g"\
			listing/listing.tex
	@cd listing; pdflatex listing.tex 
	@echo "[I] Generating its table of contents (second LaTeX pass)"
	@cd listing; pdflatex listing.tex >/dev/null
	cp listing/listing.pdf $(SHORTNAME)-listing.pdf
	@echo "[I] code listing ready"


$(SHORTNAME)-handbook.pdf: $(SHORTNAME)-refmanual.pdf handbook/*.tex
	@cat frontpage.tex handbook/handbook.tex > $(HANDBOOK)

	@sed -i 's/%ProjectName%/$(LONGNAME)/g' $(HANDBOOK)
	@sed -i "s/%DocTitle%/Programmer\'s handbook/g"\
			$(HANDBOOK)
	@sed -i "s/%RevNum%/`svn info  | grep Revision | tr -d "[:alpha:]: "`/g"\
			$(HANDBOOK)
	@make -C handbook
	cp handbook/rexio-handbook.pdf .
