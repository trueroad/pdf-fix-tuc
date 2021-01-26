//
// Fix ToUnicode CMap in PDF
// https://github.com/trueroad/pdf-fix-tuc
//
// tounicode.hh: ToUnicode CMap handler class
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

#ifndef INCLUDE_GUARD_TOUNICODE_HH
#define INCLUDE_GUARD_TOUNICODE_HH

#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

#include "regex_dispatcher_m.hh"

class tounicode: public regex_dispatcher::member_table<tounicode>
{
public:
  tounicode ();

  std::string get (void);

private:
  bool bfchar_hex (const std::smatch &);
  void bfrange_output (const int, const int, const int);
  bool bfrange_hex (const std::smatch &);
  bool beginbfchar (const std::smatch &);
  bool endbfchar (const std::smatch &);
  bool beginbfrange (const std::smatch &);
  bool endbfrange (const std::smatch &);
  bool other (const std::smatch &);

  std::stringstream ss_;
  bool is_bfchar_ = false;
  bool is_bfrange_ = false;

  const static std::unordered_map<int, int> table_;
};

#endif // INCLUDE_GUARD_TOUNICODE_HH
