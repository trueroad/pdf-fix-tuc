//
// Fix ToUnicode CMap in PDF
// https://github.com/trueroad/pdf-fix-tuc
//
// tounicode.cc: ToUnicode CMap handler class
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

#include "tounicode.hh"

#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

#include "regex_dispatcher_m.hh"

tounicode::tounicode ():
  regex_dispatcher::member_table<tounicode>
  ({
    { std::regex (R"((<[\da-fA-F]+>\s*)<([\da-fA-F]+)>(\r?))"),
      &tounicode::bfchar_hex},
    { std::regex (R"(<([\da-fA-F]+)>(\s*)<([\da-fA-F]+)>(\s*)<([\da-fA-F]+)>(\r?))"),
      &tounicode::bfrange_hex},
    { std::regex (R"((\d+)(\s*)beginbfchar(\r?))"),
      &tounicode::beginbfchar},
    { std::regex (R"((\d+)(\s*)beginbfrange(\r?))"),
      &tounicode::beginbfrange},
    { std::regex (R"(endbfchar\r?)"),
      &tounicode::endbfchar},
    { std::regex (R"(endbfrange\r?)"),
      &tounicode::endbfrange},
    { std::regex (R"(.*\r?)"),
      &tounicode::other},
  })
{
}

std::string tounicode::get (void)
{
  return ss_.str ();
}

bool tounicode::bfchar_hex (const std::smatch &sm)
{
  const auto prefix {sm[1].str ()};
  auto uni {std::stoi (sm[2].str (), nullptr, 16)};
  const auto cr {sm[3].str ()};

  if (is_bfchar_ && table_.find (uni) != table_.end ())
    {
      uni = table_.at (uni);

      ss_ << prefix
          << "<"
          << std::hex << std::uppercase
          << std::setw (4) << std::setfill ('0')
          << uni
          << std::dec << std::nouppercase
          << ">"
          << cr
          << std::endl;
    }
  else
    {
      ss_ << sm[0].str () << std::endl;
    }

  return true;
}

void tounicode::bfrange_output (const int cid_start,
                                const int cid_end,
                                const int uni_start,
                                const std::string &space1,
                                const std::string &space2,
                                const std::string &cr)
{
  ss_bfrange_ << "<"
              << std::hex << std::uppercase
              << std::setw (4) << std::setfill ('0')
              << cid_start
              << ">"
              << space1
              << "<"
              << std::setw (4) << std::setfill ('0')
              << cid_end
              << ">"
              << space2
              << "<"
              << std::setw (4) << std::setfill ('0')
              << uni_start
              << std::dec << std::nouppercase
              << ">"
              << cr
              << std::endl;
  ++bfranges_;
}

bool tounicode::bfrange_hex (const std::smatch &sm)
{
  if (!is_bfrange_)
    {
      ss_ << sm[0].str () << std::endl;
      return true;
    }

  const auto cid_start {std::stoi (sm[1].str (), nullptr, 16)};
  const auto cid_end {std::stoi (sm[3].str (), nullptr, 16)};
  const auto uni_start {std::stoi (sm[5].str (), nullptr, 16)};
  const auto space1 {sm[2].str ()};
  const auto space2 {sm[4].str ()};
  const auto cr {sm[6].str ()};

  auto cid_new_range_start = cid_start;
  auto uni_new_range_start = uni_start;
  auto cid = cid_start;
  auto uni = uni_start;
  for (; cid <= cid_end; ++cid, ++uni)
    {
      if (table_.find (uni) != table_.end ())
        {
          if (cid_new_range_start < cid)
            bfrange_output (cid_new_range_start, cid - 1,
                            uni_new_range_start,
                            space1, space2, cr);

          const auto uni_new = table_.at (uni);
          bfrange_output (cid, cid, uni_new, space1, space2, cr);

          cid_new_range_start = cid + 1;
          uni_new_range_start = uni + 1;
        }
    }

  if (cid_new_range_start == cid_start)
    {
      ss_bfrange_ << sm[0].str () << std::endl;
      ++bfranges_;
    }
  else if (cid_new_range_start < cid)
    bfrange_output (cid_new_range_start, cid - 1,
                    uni_new_range_start,
                    space1, space2, cr);

  return true;
}

bool tounicode::beginbfchar (const std::smatch &sm)
{
  const auto chars {std::stoi (sm[1].str ())};
  const auto space {sm[2].str ()};
  const auto cr {sm[3].str ()};

  if (is_bfrange_)
    {
      std::cerr << "warning: invalid ToUnicode: no endbfrange" << std::endl;
      is_bfrange_ = false;
    }
  if (is_bfchar_)
    {
      std::cerr << "warning: invalid ToUnicode: no endbfchar" << std::endl;
    }

  ss_ << chars
      << space
      << "beginbfchar"
      << cr
      << std::endl;

  is_bfchar_ = true;

  return true;
}

bool tounicode::endbfchar (const std::smatch &sm)
{
  if (is_bfrange_)
    {
      std::cerr << "warning: invalid ToUnicode: no endbfrange" << std::endl;
      is_bfrange_ = false;
    }
  if (is_bfchar_)
    {
      is_bfchar_ = false;
    }
  else
    {
      std::cerr << "warning: invalid ToUnicode: no beginbfchar" << std::endl;
    }

  ss_ << sm[0].str ()
      << std::endl;

  return true;
}

bool tounicode::beginbfrange (const std::smatch &sm)
{
  beginbfrange_space_  = sm[2].str ();
  beginbfrange_cr_ = sm[3].str ();

  if (is_bfchar_)
    {
      std::cerr << "warning: invalid ToUnicode: no endbfchar" << std::endl;
      is_bfchar_ = false;
    }
  if (is_bfrange_)
    {
      std::cerr << "warning: invalid ToUnicode: no endbfrange" << std::endl;
    }

  is_bfrange_ = true;
  ss_bfrange_.str ("");
  ss_bfrange_.clear ();
  bfranges_ = 0;

  return true;
}

bool tounicode::endbfrange (const std::smatch &sm)
{
  if (is_bfchar_)
    {
      std::cerr << "warning: invalid ToUnicode: no endbfchar" << std::endl;
      is_bfchar_ = false;
    }
  if (is_bfrange_)
    {
      is_bfrange_ = false;
    }
  else
    {
      std::cerr << "warning: invalid ToUnicode: no beginbfrange" << std::endl;
    }

  ss_ << bfranges_
      << beginbfrange_space_
      << "beginbfrange"
      << beginbfrange_cr_
      << std::endl
      << ss_bfrange_.str ()
      << sm[0].str ()
      << std::endl;
  ss_bfrange_.str ("");
  ss_bfrange_.clear ();
  bfranges_ = 0;

  return true;
}

bool tounicode::other (const std::smatch &sm)
{
  ss_ << sm[0].str ()
      << std::endl;

  return true;
}
