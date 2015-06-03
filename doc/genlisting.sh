#!/bin/bash

cat frontpage.tex 
cat genlisting.sh | tail -n 13
cd $1
echo \\pagestyle{headings}\\par\\vspace{10cm}\\section{Build essential files}\\par\\vspace{4cm}\\par
echo  \\par\\subsection{CMakeLists.txt}\\nopagebreak\\lstinputlisting{../$1/CMakeLists.txt}\\vspace{1.5cm}
echo  \\par\\subsection{installer.mak}\\nopagebreak\\lstinputlisting{../$1/installer.mak}\\vspace{1.5cm}
find  lib -name 'CMakeLists.txt' -exec echo    \\par\\subsection{{}}\\nopagebreak\\lstinputlisting{../$1/{}}\\vspace{1.5cm} \; |sort

echo \\cleardoublepage \\par\\vspace{10cm}\\section{Header files of public interface}\\par\\vspace{4cm}\\par

find  include -name '*.h++' -exec echo  \\par\\subsection{{}}\\nopagebreak\\lstinputlisting{../$1/{}}\\vspace{1.5cm} \; | grep -v \#  | grep -v fileno | sort
echo  \\par\\subsection{include/rexio/fileno\\_hack.h++ by Richard B. Kreckel}\\nopagebreak\\lstinputlisting{../$1/include/rexio/fileno_hack.h++}\\vspace{1.5cm}

echo \\cleardoublepage \\par\\vspace{10cm}\\section{Header files of internal implementation details}\\par\\vspace{4cm}\\par
find  lib -name '*.h++' -exec echo  \\par\\subsection{{}}\\nopagebreak\\lstinputlisting{../$1/{}}\\vspace{1.5cm} \; | grep -v \# | sort

echo \\cleardoublepage\\par\\vspace{10cm}\\section{C++ implementation files}\\par\\vspace{4cm}\\par
find  lib -name '*.c++' -exec echo  \\par\\subsection{{}}\\nopagebreak\\lstinputlisting{../$1/{}}\\vspace{1.5cm} \;  | grep -v \# | sort

echo \\cleardoublepage\\end{document}



exit 0



\pagebreak
\tableofcontents\vspace{2cm}\noindent{\bf Note:}\par\indent If possible, please
refer to the newest version of this source code, availble from your software
distributor. If your distribution doesn't include it, please consider changing it.

This source code is also availble from our website, and public SVN repository located
at svn://67.207.133.55/rexio.
 \vspace{\fill}\cleardoublepage
\lstset{commentstyle=\textit,stepnumber=1 ,keywordstyle=\bfseries,
    showstringspaces=false}
\lstset{numbers=left,numbersep=0.1cm,numberstyle=\tt,tabsize=4,
    basicstyle=\tt\scriptsize,texcl=false}
\lstset{language=C++,breaklines=true}