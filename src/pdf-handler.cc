//
// Fix ToUnicode CMap in PDF
// https://github.com/trueroad/pdf-fix-tuc
//
// pdf-handler.cc: PDF handler class
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

#include "pdf-handler.hh"

#include <iostream>
#include <sstream>
#include <string>

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFObjectHandle.hh>
#include <qpdf/QPDFWriter.hh>

#include "tounicode.hh"

void pdf_handler::load ()
{
  if (verbose_)
    std::cout << "Loading \"" << filename_in_ << "\"" << std::endl;

  qpdf_.processFile (filename_in_.c_str ());
}

void pdf_handler::process ()
{
  auto objs {qpdf_.getAllObjects ()};
  for (auto o: objs)
    process_obj (o);
}

void pdf_handler::save ()
{
  if (verbose_)
    std::cout << "Writing \"" << filename_out_ << "\"" << std::endl;

  QPDFWriter w (qpdf_, filename_out_.c_str ());
  w.setLinearization (linearize_);
  w.setObjectStreamMode (object_streams_);
  w.setNewlineBeforeEndstream (newline_before_endstream_);
  w.setQDFMode (qdf_);
  w.write ();
}

void pdf_handler::process_obj (QPDFObjectHandle oh)
{
  if (verbose_)
    {
      std::cout << "Object: ID " << oh.getObjectID ()
                << ", Generation " << oh.getGeneration ()
                << ", Type " << oh.getTypeName ()
                << std::endl;
    }

  if (oh.isDictionary () && oh.hasKey ("/Type"))
    {
      if (verbose_)
        std::cout << "  /Type ";

      auto type {oh.getKey ("/Type")};
      if (type.isName ())
        {
          if (verbose_)
            std::cout << type.getName () << std::endl;
          if (std::string ("/Font") == type.getName ())
            process_font (oh);
        }
      else
        {
          if(verbose_)
            std::cout << "does not contain a name" << std::endl;
        }
    }
}

void pdf_handler::process_font (QPDFObjectHandle oh)
{
  if (oh.hasKey ("/ToUnicode"))
    {
      if (verbose_)
        std::cout << "  /ToUnicode ";

      auto tounicode {oh.getKey ("/ToUnicode")};
      if (tounicode.isStream ())
        {
          if (verbose_)
            std::cout << "stream" << std::endl;
          process_tounicode (tounicode);
        }
      else
        {
          if(verbose_)
            std::cout << "does not contain a stream" << std::endl;
        }
    }
}

void pdf_handler::process_tounicode (QPDFObjectHandle oh)
{
  std::stringstream ss_in;
  {
    auto pb {oh.getStreamData ()};
    std::string buff (reinterpret_cast<const char*> (pb->getBuffer ()),
                      pb->getSize ());
    ss_in.str (buff + "\n");
  }

  tounicode tu;
  tu.process_stream (ss_in);

  auto in {ss_in.str ()};
  in.pop_back ();
  auto out {tu.get ()};
  out.pop_back ();

  if (in != out)
    {
      if (verbose_)
        std::cout << "  replacing..." << std::endl;

      oh.replaceStreamData (out,
                            QPDFObjectHandle::newNull(),
                            QPDFObjectHandle::newNull());
    }
}
