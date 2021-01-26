TESTS = $(TESTPDFTOTEXT)

TEST_EXTENSIONS = .pdftotext

PDFTOTEXT_LOG_COMPILER = $(srcdir)/test-pdftotext.sh

TESTS_ENVIRONMENT = top_builddir=$(top_builddir) \
	srcdir=$(srcdir) \
	DIFF=$(DIFF) \
	PDFTOTEXT=$(PDFTOTEXT)

# Source files for test PDFs
TESTTEXS = bfchar.tex
# Test PDFs
TESTPDFS = $(TESTTEXS:.tex=.pdf) bfrange.pdf
# Garbled text
TESTGARBLED = $(TESTPDFS:.pdf=-garbled.txt)
# Expected text
TESTEXPECTED = $(TESTPDFS:.pdf=-expected.txt)

# Tests by pdftotext
TESTPDFTOTEXT = $(TESTPDFS:.pdf=-testtext.pdftotext)

# Test scripts and test PDFs
EXTRA_DIST = test-pdftotext.sh \
	$(TESTTEXS) $(TESTPDFS) $(TESTGARBLED) $(TESTEXPECTED) \
	build_bfrange.py

# For `make clean'
CLEANFILES = $(TESTTEXS:.tex=.log) \
	$(TESTPDFTOTEXT) \
	$(TESTPDFS:.pdf=-test.pdf)

# Generate garbled PDFs
# Tarball contains them for convenience.
%.dvi: %.tex
	$(UPTEX) -halt-on-error -interaction=nonstopmode -file-line-error $<

%.pdf: %.dvi %-garbled.txt SourceHanSerif-Regular-UTF16
	if [ ! -e SourceHanSerif-Regular-UTF16 ]; then \
		ln -s $(srcdir)/SourceHanSerif-Regular-UTF16 ; \
	fi
	$(DVIPDFMX) $< -o $@
	$(PDFTOTEXT) $@ $(@:.pdf=-extracted.txt)
	$(DIFF) -ubwBE $(word 2,$^) $(@:.pdf=-extracted.txt)

# Generate bfrange PDF
# Tarball contains them for convenience.
bfrange.pdf: bfchar.pdf
	$(QPDF) --qdf $< bfchar.qdf
	$(srcdir)/build_bfrange.py < bfchar.qdf > bfrange.qdf
	$(FIX_QDF) < bfrange.qdf > $@

# Test PDFs
%-test.pdf: %.pdf
	${top_builddir}/src/pdf-fix-tuc $< $@

# Test pdftotext
%-testtext.pdftotext: %-test.pdf %-expected.txt
	$(PDFTOTEXT) $< $@

.PRECIOUS: %.dvi %.pdf %-test.pdf %-testtext.pdftotext
