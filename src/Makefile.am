bin_PROGRAMS = pdf-fix-tuc
pdf_fix_tuc_SOURCES = main.cc \
	pdf-handler.cc pdf-handler.hh \
	tounicode.cc tounicode.hh tounicode_table.cc \
	cmdlineparse.hh regex_dispatcher_m.hh
pdf_fix_tuc_LDADD = $(LIBQPDF_LIBS)

EXTRA_DIST = build_table.py

if ENABLE_BUILD_TABLE
tounicode_table.cc: aj17-kanji.txt
	$(srcdir)/build_table.py $< > $@
endif

aj17-kanji.txt:
	$(WGET) https://github.com/adobe-type-tools/Adobe-Japan1/raw/master/aj17-kanji.txt
