//
// Fix ToUnicode CMap in PDF
// https://github.com/trueroad/pdf-fix-tuc
//
// pdf-handler.hh: PDF handler class
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

#ifndef INCLUDE_GUARD_PDF_HANDLER_HH
#define INCLUDE_GUARD_PDF_HANDLER_HH

#include <string>

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFObjectHandle.hh>

class pdf_handler
{
public:
  pdf_handler ()
  {
  }
  void set_verbose (bool v)
  {
    verbose_ = v;
  }
  void set_filename_in (const std::string &f)
  {
    filename_in_ = f;
  }
  void set_filename_out (const std::string &f)
  {
    filename_out_ = f;
  }
  void set_linearize (bool l)
  {
    linearize_ = l;
  }
  void set_object_streams (qpdf_object_stream_e os)
  {
    object_streams_ = os;
  }
  void set_newline_before_endstream (bool n)
  {
    newline_before_endstream_ = n;
  }
  void set_qdf (bool q)
  {
    qdf_ = q;
  }

  void load ();
  void process ();
  void save ();

private:
  void process_obj (QPDFObjectHandle oh);
  void process_font (QPDFObjectHandle oh);
  void process_tounicode (QPDFObjectHandle oh);

  std::string filename_in_;
  std::string filename_out_;

  QPDF qpdf_;

  bool verbose_ = false;

  bool linearize_ = false;
  qpdf_object_stream_e object_streams_ = qpdf_o_preserve;
  bool newline_before_endstream_ = false;
  bool qdf_ = false;
};

#endif // INCLUDE_GUARD_PDF_HANDLER_HH
