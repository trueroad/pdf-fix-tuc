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
TESTPDFS = $(TESTTEXS:.tex=.pdf) bfrange.pdf bfrange2.pdf
# Garbled text
TESTGARBLED = $(TESTPDFS:.pdf=-garbled.txt)
# Expected text
TESTEXPECTED = $(TESTPDFS:.pdf=-expected.txt)

# Tests by pdftotext
TESTPDFTOTEXT = $(TESTPDFS:.pdf=-testtext.pdftotext)

# Test scripts and test PDFs
EXTRA_DIST = test-pdftotext.sh \
	$(TESTTEXS) $(TESTPDFS) $(TESTGARBLED) $(TESTEXPECTED) \
	SourceHanSerif-Regular-UTF16 \
	build_bfrange.py build_bfrange2.py

# For `make clean'
CLEANFILES = $(TESTTEXS:.tex=.log) \
	$(TESTPDFTOTEXT) \
	$(TESTPDFS:.pdf=-test.pdf)

if GENERATE_TEST_PDF
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
bfchar.qdf: bfchar.pdf
	$(QPDF) --qdf $< $@

bfrange.qdf: bfchar.qdf
	$(srcdir)/build_bfrange.py < $< > $@

bfrange.pdf: bfrange.qdf
	$(FIX_QDF) < $< > $@

# Generate bfrange2 PDF
# Tarball contains them for convenience.
bfrange2.qdf: bfrange.qdf
	$(srcdir)/build_bfrange2.py < $< > $@

bfrange2.pdf: bfrange2.qdf
	$(FIX_QDF) < $< > $@
endif

# Test PDFs
%-test.pdf: %.pdf
	${top_builddir}/src/pdf-fix-tuc $< $@

# Test pdftotext
%-testtext.pdftotext: %-test.pdf %-expected.txt
	$(PDFTOTEXT) $< $@

.PRECIOUS: %.dvi %.pdf %-test.pdf %-testtext.pdftotext
