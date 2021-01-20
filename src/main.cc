//
// Fix ToUnicode CMap in PDF
// https://github.com/trueroad/pdf-fix-tuc
//
// Copyright (C) 2021 Masamichi Hosoda.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//

#include "config.h"

#include <exception>
#include <iostream>
#include <string>

#include "cmdlineparse.hh"
#include "pdf-handler.hh"

bool parse_options (int argc, char *argv[], pdf_handler *pph)
{
  cmdlineparse::parser cmd;

  cmd.set_usage_unamed_opts ("INPUT.pdf OUTPUT.pdf");
  cmd.add_default ();

  bool verbose;
  cmd.add_flag (0, "verbose", &verbose, "    Verbose");

  bool linearize;
  std::string object_streams;
  bool newline_before_endstream;
  bool qdf;
  cmd.add_flag (0, "linearize", &linearize,
                "    Output linearized (web-optimized) PDF",
                "Output PDF settings (QPDF)");
  cmd.add_string (0, "object-streams", &object_streams, "preserve",
                  "    Settings for object streams",
                  "[preserve|disable|generate]",
                  "Output PDF settings (QPDF)");
  cmd.add_flag (0, "newline-before-endstream", &newline_before_endstream,
                "    Output newline before endstream",
                "Output PDF settings (QPDF)");
  cmd.add_flag (0, "qdf", &qdf,
                "    Output QDF",
                "Output PDF settings (QPDF)");

  if (!cmd.parse (argc, argv))
    return false;

  auto uargs {cmd.get_unamed_args ()};
  if (uargs.size () != 2)
    {
      std::cout << cmd.build_help ();
      return false;
    }

  std::cout << cmd.get_version_string () << std::endl;

  pph->set_verbose (verbose);

  pph->set_linearize (linearize);
  if (object_streams == "preserve")
    pph->set_object_streams (qpdf_o_preserve);
  else if (object_streams == "generate")
    pph->set_object_streams (qpdf_o_generate);
  else if (object_streams == "disable")
    pph->set_object_streams (qpdf_o_disable);
  else
    {
      std::cerr << "unknwon --object-streams mode" << std::endl;
      return false;
    }

  pph->set_newline_before_endstream (newline_before_endstream);
  pph->set_qdf (qdf);

  pph->set_filename_in (uargs[0]);
  pph->set_filename_out (uargs[1]);

  return true;
}

int main (int argc, char *argv[])
{
  try
    {
      pdf_handler ph;
      if (!parse_options (argc, argv, &ph))
        return false;

      ph.load ();
      ph.process ();
      ph.save ();

      std::cout << "Done." << std::endl;
    }
  catch (std::exception &e)
    {
      std::cerr << e.what () << std::endl;
      return 1;
    }

  return 0;
}
