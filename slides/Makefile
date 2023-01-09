SVGS := $(wildcard images/*.svg)
PDFIMGS := $(patsubst %.svg, %.pdf, $(SVGS))
LECTURE_PREFIX ?= lecture% bt% mt%

default: all

all: svg2pdf slides handouts tablet notes article

svg2pdf: $(PDFIMGS)

%.pdf: %.svg
	cairosvg --format pdf --output $@ $<

slides:
ifeq ( , $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)))
	@latexmk -lualatex -CF -usepretex="\PassOptionsToClass{final}{bfhbeamer}" -jobname="%A-slides"
	@echo '** [DONE] All available TEX files compiled in presentation mode'
else
	@latexmk -lualatex -CF -usepretex="\PassOptionsToClass{final}{bfhbeamer}" -jobname="%A-slides" $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS))*
	@echo '** [DONE] The $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)) file compiled in presentation mode'
endif

handouts:
ifeq ( , $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)))
	@latexmk -lualatex -CF -usepretex="\PassOptionsToClass{paperNotes}{bfhbeamer}" -jobname="%A-print"
	@echo '** [DONE] All available TEX files compiled in paperNotes mode'
else
	@latexmk -lualatex -CF -usepretex="\PassOptionsToClass{paperNotes}{bfhbeamer}" -jobname="%A-print" $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS))*
	@echo '** [DONE] The $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)) file compiled in paperNotes mode'
endif

notes:
ifeq ( , $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)))
	@latexmk -lualatex -CF -usepretex="\PassOptionsToClass{printNotes}{bfhbeamer}" -jobname="%A-notes"
	@echo '** [DONE] All available TEX files compiled in handouts with notes mode'
else
	@latexmk -lualatex -CF -usepretex="\PassOptionsToClass{printNotes}{bfhbeamer}" -jobname="%A-notes" $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS))*
	@echo '** [DONE] The $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)) file compiled in handouts with notes mode'
endif

tablet:
ifeq ( , $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)))
	@latexmk -lualatex -usepretex="\PassOptionsToClass{tabletNotes}{bfhbeamer}" -jobname="%A-tablet"
	@echo '** [DONE] All available TEX files compiled in table mode'
else
	@latexmk -lualatex -usepretex="\PassOptionsToClass{tabletNotes}{bfhbeamer}" -jobname="%A-tablet" $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS))*
	@echo '** [DONE] The $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)) file compiled in tablet mode'
endif

article:
ifeq ( , $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)))
	@latexmk -lualatex -usepretex="\PassOptionsToClass{beamerarticle}{bfhbeamer}" -jobname="%A-reader"
	@echo '** [DONE] All available TEX files compiled in article mode'
else
	@latexmk -lualatex -usepretex="\PassOptionsToClass{beamerarticle}{bfhbeamer}" -jobname="%A-reader" $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS))*
	@echo '** [DONE] The $(filter $(LECTURE_PREFIX),$(MAKECMDGOALS)) file compiled in article mode'
endif


%:
	@:

clean:
	@latexmk -c

mrproper:
	@find . -iname '*~' -exec rm {} \;
	@latexmk -C

distclean:
	@rm -rf _build
