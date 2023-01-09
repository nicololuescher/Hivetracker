## How to build using LaTeXmk

 * Build beamer presentation mode (slides) of FILE.tex. If there is no *.tex file passed to the command all tex files will be processed.
 ```
  latexmk -lualatex  -pv -usepretex="\PassOptionsToClass{final}{bfhbeamer}" -jobname="%A-slides" FILE.tex
 ```
 * Build beamer handout mode (optimized for tablet use) of FILE.tex. If there is no *.tex file passed to the command all tex files will be processed.
 ```
  latexmk -lualatex  -pv -usepretex="\PassOptionsToClass{tabletNotes}{bfhbeamer}" -jobname="%A-tablet" FILE.tex
 ```
 * Build beamer handout mode (optimized for printout use) of FILE.tex. If there is no *.tex file passed to the command all tex files will be processed.
 ```
  latexmk -lualatex  -pv -usepretex="\PassOptionsToClass{paperNotes}{bfhbeamer}" -jobname="%A-print" FILE.tex
 ```
 * Build beamer handout with notes -- include the content of the note environment within a frame or outside a frame environment. If there is no *.tex file passed to the command all tex files will be processed.
 ```
 latexmk -lualatex -CF -usepretex="\PassOptionsToClass{printNotes}{bfhbeamer}" -jobname="%A-notes"
 ``` 
 * Build beamer in article mode (optimized as a reader) of FILE.tex. If there is no *.tex file passed to the command all tex files will be processed.
 ```
  latexmk -lualatex  -pv -usepretex="\PassOptionsToClass{type=beamerarticle}{bfhbeamer}" -jobname="%A-reader" FILE.tex
 ```
