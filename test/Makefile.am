TESTS = $(TESTPDFTOTEXT)

TEST_EXTENSIONS = .pdftotext

PDFTOTEXT_LOG_COMPILER = $(srcdir)/test-pdftotext.sh

TESTS_ENVIRONMENT = top_builddir=$(top_builddir) \
	srcdir=$(srcdir) \
	DIFF=$(DIFF) \
	PDFTOTEXT=$(PDFTOTEXT)

# Source files for test PDFs
TESTTEXS = test.tex
# Test PDFs
TESTPDFS = $(TESTTEXS:.tex=.pdf)
# Garbled text
TESTGARBLED = $(TESTTEXS:.tex=-garbled.txt)
# Expected text
TESTEXPECTED = $(TESTTEXS:.tex=-expected.txt)

# Tests by pdftotext
TESTPDFTOTEXT = $(TESTPDFS:.pdf=-testtext.pdftotext)

# Test scripts and test PDFs
EXTRA_DIST = test-pdftotext.sh \
	$(TESTTEXS) $(TESTPDFS) $(TESTGARBLED) $(TESTEXPECTED)

# For `make clean'
CLEANFILES = $(TESTTEXS:.tex=.log) \
	$(TESTPDFTOTEXT) \
	$(TESTPDFS:.pdf=-test.pdf)

# Generate garbled PDFs
# Tarball contains them for convenience.
%.dvi: %.tex
	$(UPTEX) -halt-on-error -interaction=nonstopmode -file-line-error $<

%.pdf: %.dvi %-garbled.txt SourceHanSerif-Regular-UTF16
	if [ ! -e $(notdir $(word 3,$^)) ]; then \
		ln -s $(word 3,$^); \
	fi
	$(DVIPDFMX) $< -o $@
	$(PDFTOTEXT) $@ $(@:.pdf=-extracted.txt)
	$(DIFF) -ubwBE $(word 2,$^) $(@:.pdf=-extracted.txt)

# Test PDFs
%-test.pdf: %.pdf
	${top_builddir}/src/pdf-fix-tuc $< $@

# Test pdftotext
%-testtext.pdftotext: %-test.pdf %-expected.txt
	$(PDFTOTEXT) $< $@

.PRECIOUS: %.dvi %.pdf %-test.pdf %-testtext.pdftotext
