//
// Regex dispatcher table for C++11
// https://gist.github.com/trueroad/bdb81622c083ee544698c4bc55cda6a9
//
// regex_dispatcher_m.hh: Regex dispatcher table class by member function
//
// Copyright (C) 2018 Masamichi Hosoda.
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

#ifndef INCLUDE_GUARD_REGEX_DISPATCHER_M_HH_
#define INCLUDE_GUARD_REGEX_DISPATCHER_M_HH_

#include <istream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace regex_dispatcher
{
  template <class Derived>
  class member_table
  {
    using callback_type = bool (Derived::*) (const std::smatch &);
    using table_type =
      std::vector<std::pair<std::regex, callback_type>>;

  public:
    member_table (table_type t):
      table_ (t)
    {
    }

    void process_line (const std::string &s)
    {
      std::smatch sm;
      for (const auto &t: table_)
        {
          if (std::regex_match (s, sm, t.first))
            {
              if ((static_cast<Derived*> (this)->*(t.second)) (sm))
                break;
            }
        }
    }

    void process_stream (std::istream &is)
    {
      std::string line;
      while (std::getline (is, line))
        {
          process_line (line);
        }
    }

  private:
    table_type table_;
  };
}

#endif // INCLUDE_GUARD_REGEX_DISPATCHER_M_HH_
