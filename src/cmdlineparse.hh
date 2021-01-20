//
// One header file Commandline Parse for C++11 2019-11-17.07
// https://github.com/trueroad/cmdlineparse
//
// Copyright (C) 2016, 2017, 2019 Masamichi Hosoda. All rights reserved.
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

#ifndef INCLUDE_GUARD_CMDLINEPARSE_HH
#define INCLUDE_GUARD_CMDLINEPARSE_HH

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>

// gettext macros
#ifndef _

#define _(DUMMY) DUMMY
#define CMDLINEPARSE_HH_DEFINE_DUMMY_GETTEXT

#endif

namespace cmdlineparse
{

  enum class arg_mode
    {
      no_argument = 0,
      required_argument = 1,
      optional_argument = 2
    };

  enum class abort_reason
    {
      option_handler = -1,
      no_abort = 0,
      error_extra_arg,
      error_no_arg,
      error_ambiguous_option,
      error_unknown_option,
      error_no_arg_short,
      error_unknown_option_short,
    };

  class parser
  {
  public:
    // Add an option handler
    bool add_handler (char short_name, const std::string &long_name,
                      arg_mode has_arg,
                      std::function<bool(const std::string&)> option_handler,
                      const std::string &description = "",
                      const std::string &typestr = "",
                      const std::string &header = "",
                      const std::string &group = "");

    // Add a flag option
    bool add_flag (char short_name, const std::string &long_name,
                   bool *flag,
                   const std::string &description = "",
                   const std::string &group = "");

    // Add a string option
    bool add_string (char short_name, const std::string &long_name,
                     std::string *var, const std::string &defval,
                     const std::string &description = "",
                     const std::string &typestr = "STRING",
                     const std::string &group = "");

    // Add help description
    void add_description (char short_name, const std::string &long_name,
                          arg_mode has_arg,
                          const std::string &description,
                          const std::string &typestr = "",
                          const std::string &header = "",
                          const std::string &group = "");

    // Add default handler
    bool add_default_help (void);
    bool add_default_version (void);
    bool add_default (void)
    {
      if (!add_default_help ())
        return false;
      return add_default_version ();
    }

    // Parse options
    bool parse (int argc, char const* const* argv, int optind = 1);

    // Build default strings
    std::string build_usage (void) const;
    std::string build_help (void) const;

    // Get version_string
    const std::string &get_version_string () const noexcept
    {
      return version_string;
    }

    // Set version_string
    void set_version_string (const std::string &s)
    {
      version_string = s;
    }

    // Get unamed_args
    const std::vector<std::string> &get_unamed_args () const noexcept
    {
      return unamed_args;
    }

    // Set usage_unamed_opts
    void set_usage_unamed_opts (const std::string &s)
    {
      usage_unamed_opts = s;
    }

    // Get abort reason
    abort_reason get_abort_reason () const noexcept
    {
      return abort;
    }

    // Get abort option
    const std::string &get_abort_option () const noexcept
    {
      return abort_option;
    }

    // Set opterr
    void set_opterr (bool flag) noexcept
    {
      opterr = flag;
    }

    // Set long_only
    void set_long_only (bool flag) noexcept
    {
      long_only = flag;
    }

    // Set continue_on_error
    void set_continue_on_error (bool flag) noexcept
    {
      continue_on_error = flag;
    }

    // Set abbreviated_long_name
    void set_abbreviated_long_name (bool flag) noexcept
    {
      abbreviated_long_name = flag;
    }

    // Constructor
    parser ();

    // Const
    const std::string h_indent {"  "};
    const std::string h_space {"   "};
    const std::string d_indent {"    "};

  private:
    // Internal functions
    void add_short (char short_name, arg_mode has_arg,
                    std::function<bool(const std::string&)> option_handler);
    void add_long (const std::string &long_name, arg_mode has_arg,
                    std::function<bool(const std::string&)> option_handler);
    bool find_unique_long_name (const std::string &long_name,
                                std::pair<arg_mode,
                                std::function<bool(const std::string&)>>
                                *target,
                                bool *ambiguous);
    bool parse_long_name (std::vector<std::string>::const_iterator *it);
    bool parse_short_name (std::vector<std::string>::const_iterator *it);

    // Error handlers
    std::function<bool(const std::string&,
                       const std::string&)> error_extra_arg
    {
      [this](const std::string &long_name, const std::string &)->bool
        {
          if (opterr)
            std::cerr << argvs[0]
                      << _(": option doesn't take an argument -- ")
                      << long_name << std::endl;
          return continue_on_error;
        }
    };
    std::function<bool(const std::string&)> error_no_arg
    {
      [this](const std::string &long_name)->bool
        {
          if (opterr)
            std::cerr << argvs[0]
                      << _(": option requires an argument -- ")
                      << long_name << std::endl;
          return continue_on_error;
        }
    };
    std::function<bool(const std::string&)> error_ambiguous_option
    {
      [this](const std::string &optchars)->bool
        {
          if (opterr)
            std::cerr << argvs[0]
                      << _(": ambiguous option -- ")
                      << optchars << std::endl;
          return continue_on_error;
        }
    };
    std::function<bool(const std::string&)> error_unknown_option
    {
      [this](const std::string &optchars)->bool
        {
          if (opterr)
            std::cerr << argvs[0]
                      << _(": unknown option -- ")
                      << optchars << std::endl;
          return continue_on_error;
        }
    };
    std::function<bool(char)> error_no_arg_short
    {
      [this](char optchar)->bool
        {
          if (opterr)
            std::cerr << argvs[0]
                      << _(": option requires an argument -- ")
                      << optchar << std::endl;
          return continue_on_error;
        }
    };
    std::function<bool(char)> error_unknown_option_short
    {
      [this](char optchar)->bool
        {
          if (opterr)
            std::cerr << argvs[0]
                      << _(": unknown option -- ")
                      << optchar << std::endl;
          return continue_on_error;
        }
    };

    // Help strings
    std::string version_string;
    std::string usage_unamed_opts;

    // Flags
    bool opterr {true};
    bool continue_on_error {false};
    bool long_only {false};
    bool abbreviated_long_name {true};

    // Abort reason
    abort_reason abort {abort_reason::no_abort};
    std::string abort_option;

    // Arguments
    std::vector<std::string> argvs;
    std::vector<std::string> unamed_args;

    // Maps
    std::map<char,
             std::pair<arg_mode,
                       std::function <bool(const std::string&)>>> short_map;
    std::map<std::string,
             std::pair<arg_mode,
                       std::function <bool(const std::string&)>>> long_map;
    std::map<std::string, std::vector<std::string>> help_map;
  };

  inline
  parser::parser ()
  {
#ifdef PACKAGE_STRING
    // Build version_string
    std::stringstream ss;
    ss << PACKAGE_STRING << std::endl;

#ifdef PACKAGE_COPYRIGHT
    ss << PACKAGE_COPYRIGHT << std::endl;
#ifdef PACKAGE_LICENSE
    ss << PACKAGE_LICENSE << std::endl;
#endif // PACKAGE_LICENSE
#endif // PACKAGE_COPYRIGHT

#ifdef PACKAGE_URL
    ss << std::endl << PACKAGE_URL << std::endl;
#endif // PACKAGE_URL

    version_string = ss.str ();
#endif // PACKAGE_STRING
  }

  inline void
  parser::add_short (char short_name, arg_mode has_arg,
                     std::function<bool(const std::string&)> option_handler)
  {
    if (short_name)
      {
        short_map[short_name] =
          std::pair<arg_mode, std::function <bool(const std::string&)>>
          (has_arg, option_handler);
      }
  }

  inline void
  parser::add_long (const std::string &long_name, arg_mode has_arg,
                    std::function<bool(const std::string&)> option_handler)
  {
    if (!long_name.empty ())
      {
        long_map[long_name] =
          std::pair<arg_mode, std::function <bool(const std::string&)>>
          (has_arg, option_handler);
      }
  }

  inline void
  parser::add_description (char short_name, const std::string &long_name,
                           arg_mode has_arg,
                           const std::string &description,
                           const std::string &typestr,
                           const std::string &header,
                           const std::string &group)
  {
    std::stringstream ss;

    if (short_name || !long_name.empty () || !header.empty ())
      {
        ss << h_indent;
        if (short_name)
          {
            ss << "-" << short_name;
            if (long_name.empty () && !typestr.empty ())
              ss << typestr;
          }
        if (!long_name.empty ())
          {
            if (short_name)
              ss << ", ";
            ss << "--" << long_name;
            if (!typestr.empty ())
              {
                switch (has_arg)
                  {
                  case arg_mode::no_argument:
                    // Nothing to do
                    break;
                  case arg_mode::required_argument:
                    ss << "=" << typestr;
                    break;
                  case arg_mode::optional_argument:
                    ss << "[=" << typestr << "]";
                    break;
                  }
              }
          }
        if (!header.empty ())
          ss << header;
        ss << std::endl;
      }

    if (!description.empty ())
      ss << description << std::endl;

    help_map[group].push_back (ss.str ());
  }

  inline bool
  parser::add_handler (char short_name, const std::string &long_name,
                       arg_mode has_arg,
                       std::function<bool(const std::string&)> option_handler,
                       const std::string &description,
                       const std::string &typestr,
                       const std::string &header,
                       const std::string &group)
  {
    if (short_name && short_map.find (short_name) != short_map.end ())
      {
        // The short name is already registerd.
        return false;
      }
    if (!long_name.empty() && long_map.find (long_name) != long_map.end ())
      {
        // The long name is already registerd.
        return false;
      }

    add_short (short_name, has_arg, option_handler);
    add_long (long_name, has_arg, option_handler);

    if (!description.empty () || !typestr.empty ())
      add_description (short_name, long_name, has_arg,
                       description, typestr, header, group);

    return true;
  }

  inline bool
  parser::add_flag (char short_name, const std::string &long_name,
                    bool *flag,
                    const std::string &description,
                    const std::string &group)
  {
    *flag = false;
    return add_handler (short_name, long_name, arg_mode::no_argument,
                        [flag](const std::string &)->bool
                        {
                          *flag = true;
                          return true;
                        },
                        description, "", "", group);
  }

  inline bool
  parser::add_string (char short_name, const std::string &long_name,
                      std::string *var, const std::string &defval,
                      const std::string &description,
                      const std::string &typestr,
                      const std::string &group)
  {
    *var = defval;
    std::string header;

    if(!defval.empty ())
      {
        // Three spaces for separator (same as h_space)
        header = _("   (default=") + defval + ")";
      }
    return add_handler (short_name, long_name, arg_mode::required_argument,
                        [var](const std::string &optarg)->bool
                        {
                          *var = optarg;
                          return true;
                        },
                        description, typestr, header, group);
  }

  inline bool
  parser::add_default_help (void)
  {
    return add_handler ('h', "help", arg_mode::no_argument,
                        [this](const std::string &)->bool
                        {
                          std::cout << build_help ();
                          return false;
                        },
                        // Four spaces for indent (same as d_indent)
                        _("    Print help and exit"));
  }

  inline bool
  parser::add_default_version (void)
  {
    return add_handler ('V', "version", arg_mode::no_argument,
                        [this](const std::string &)->bool
                        {
                          std::cout << version_string;
                          return false;
                        },
                        // Four spaces for indent (same as d_indent)
                        _("    Print version and exit"));
  }

  inline bool
  parser::find_unique_long_name (const std::string &long_name,
                                 std::pair<arg_mode,
                                 std::function<bool(const std::string&)>>
                                 *target,
                                 bool *ambiguous)
  {
    if (long_map.find (long_name) != long_map.end ())
      {
        // Long option name found
        *target = long_map[long_name];
        return true;
      }
    if (abbreviated_long_name)
      {
        // Search abbreviated long option name
        for (decltype (long_map.cbegin()) it =
               long_map.upper_bound (long_name);
             it != long_map.cend ();
             ++it)
          {
            if (it->first.substr (0, long_name.size ()) == long_name)
              {
                auto next_it = it;
                ++next_it;
                if (next_it != long_map.cend () &&
                    next_it->first.substr (0, long_name.size ()) == long_name)
                  {
                    // Failed: ambiguous option
                    *ambiguous = true;
                    return false;
                  }
                // Unique abbreviated long option name fount
                *target = it->second;
                return true;
              }
          }
      }
    // Failed: unknown long option name
    *ambiguous = false;
    return false;
  }

  inline bool
  parser::parse_long_name (std::vector<std::string>::const_iterator *it)
  {
    size_t optchars_pos = (*it)->at(1) == '-' ?
      2: // Double hyphen (--long_name) style
      1; // Single hyphen (-long_name) style
    auto delimiter_pos = (*it)->find ('=');
    std::string long_name = (*it)->substr (optchars_pos,
                                           delimiter_pos - optchars_pos);
    std::string optarg;

    if (delimiter_pos != std::string::npos)
      {
        // Option characters have an option argument
        // (something like --long_name=optarg)
        optarg = (*it)->substr (delimiter_pos + 1, std::string::npos);
      }

    std::pair<arg_mode, std::function<bool(const std::string&)>> target;
    bool ambiguous;
    if (find_unique_long_name (long_name, &target, &ambiguous))
      {
        // Long option name found
        switch (target.first)
          {
          case arg_mode::no_argument:
            if (delimiter_pos != std::string::npos)
              {
                if (!error_extra_arg (long_name, optarg))
                  {
                    abort = abort_reason::error_extra_arg;
                    abort_option = long_name;
                    return false;
                  }
                return true;
              }
            break;
          case arg_mode::required_argument:
            if (delimiter_pos == std::string::npos)
              {
                if ((*it + 1) != argvs.cend ())
                  {
                    // Next argv-element is an option argument
                    // (something like --long_name optarg)
                    optarg = *(++(*it));
                  }
                else
                  {
                    if (!error_no_arg (long_name))
                      {
                        abort = abort_reason::error_no_arg;
                        abort_option = long_name;
                        return false;
                      }
                    return true;
                  }
              }
            break;
          case arg_mode::optional_argument:
            // Nothing to do
            break;
          }
        // Call option handler
        if (!target.second (optarg))
          {
            abort = abort_reason::option_handler;
            abort_option = long_name;
            return false;
          }
        return true;
      }

    // Long option name did not find
    if (optchars_pos == 1)
      {
        // Fallback to short option name
        return parse_short_name (it);
      }
    if (ambiguous)
      {
        if (!error_ambiguous_option ((*it)->substr (optchars_pos,
                                                    std::string::npos)))
          {
            abort = abort_reason::error_ambiguous_option;
            abort_option = long_name;
            return false;
          }
        return true;
      }
    if (!error_unknown_option ((*it)->substr (optchars_pos,
                                              std::string::npos)))
      {
        abort = abort_reason::error_unknown_option;
        abort_option = long_name;
        return false;
      }
    return true;
  }

  inline bool
  parser::parse_short_name (std::vector<std::string>::const_iterator *it)
  {
    for (auto name_it = (*it)->cbegin () + 1;
         name_it != (*it)->cend ();
         ++name_it)
      {
        if (short_map.find (*name_it) != short_map.end())
          {
            // Short option name found
            auto target = short_map[*name_it];
            switch (target.first)
              {
              case arg_mode::no_argument:
                // Option characters doesn't have an option argument
                // (something like -o)
                if (!target.second (""))  // Call option handler
                  {
                    abort = abort_reason::option_handler;
                    abort_option = *name_it;
                    return false;
                  }
                break;
              case arg_mode::required_argument:
                if ((name_it + 1) != (*it)->cend ())
                  {
                    // Option characters have an option argument
                    // (something like -ooptarg)
                    std::string optarg (name_it + 1, (*it)->cend ());
                    if (!target.second (optarg))  // Call option handler
                      {
                        abort = abort_reason::option_handler;
                        abort_option = *name_it;
                        return false;
                      }
                    return true;
                  }
                else if ((*it + 1) != argvs.cend ())
                  {
                    // Next argv-element is an option argument
                    // (something like -o optarg)
                    std::string optarg = *(++(*it));
                    if (!target.second (optarg))  // Call option handler
                      {
                        abort = abort_reason::option_handler;
                        abort_option = *name_it;
                        return false;
                      }
                    return true;
                  }
                else
                  {
                    if (!error_no_arg_short (*name_it))
                      {
                        abort = abort_reason::error_no_arg_short;
                        abort_option = *name_it;
                        return false;
                      }
                  }
                break;
              case arg_mode::optional_argument:
                if ((name_it + 1) != (*it)->cend ())
                  {
                    // Option characters have an option argument
                    // (something like -ooptarg)
                    std::string optarg (name_it + 1, (*it)->cend ());
                    if (!target.second (optarg))  // Call option handler
                      {
                        abort = abort_reason::option_handler;
                        abort_option = *name_it;
                        return false;
                      }
                    return true;
                  }
                // Option characters doesn't have an option argument
                // (something like -o)
                if (!target.second (""))  // Call option handler
                  {
                    abort = abort_reason::option_handler;
                    abort_option = *name_it;
                    return false;
                  }
                break;
              }
          }
        else
          {
            if (!error_unknown_option_short (*name_it))
              {
                abort = abort_reason::error_unknown_option_short;
                abort_option = *name_it;
                return false;
              }
          }
      }
    return true;
  }

  inline bool
  parser::parse (int argc, char const* const* argv, int optind)
  {
    argvs.assign (argv, argv + argc);

    if (version_string.empty ())
      version_string = argvs[0] + "\n";

    bool all_skip = false;

    for (auto argv_it = argvs.cbegin () + optind;
         argv_it != argvs.cend ();
         ++argv_it)
      {
        // Check skip
        if (all_skip)
          {
            unamed_args.push_back (*argv_it);
            continue;
          }

        // Check "--" and "-"
        // They are not option element.
        if (*argv_it == "--")
          {
            all_skip = true;
            continue;
          }
        else if (*argv_it == "-")
          {
            unamed_args.push_back (*argv_it);
            continue;
          }

        // Check option element
        if (argv_it->substr (0, 2) == "--")
          {
            // Long option
            if (!parse_long_name (&argv_it))
              return false;
          }
        else if (argv_it->substr (0, 1) == "-")
          {
            if (long_only)
              {
                // Long option
                if (!parse_long_name (&argv_it))
                  return false;
              }
            else
              {
                // Short option
                if (!parse_short_name (&argv_it))
                  return false;
              }
          }
        else
          {
            // It is not an option element.
            unamed_args.push_back (*argv_it);
          }
      }
    return true;
  }

  inline std::string
  parser::build_usage (void) const
  {
    std::stringstream ss;

    ss << _("Usage: ") << argvs[0] << _(" [options] ");
    if (!usage_unamed_opts.empty ())
      ss << "[" << usage_unamed_opts << "] ";
    ss << "..." << std::endl;

    return ss.str ();
  }

  inline std::string
  parser::build_help (void) const
  {
    std::stringstream ss;

    ss << version_string << std::endl << build_usage () << std::endl;

    for (const auto &group: help_map)
      {
        if (!group.first.empty ())
          ss << std::endl << group.first << ":" << std::endl;
        for (const auto &description: group.second)
          {
            ss << description;
          }
      }

    return ss.str ();
  }

}

#if defined (_) && defined (CMDLINEPARSE_HH_DEFINE_DUMMY_GETTEXT)
#undef _
#undef CMDLINEPARSE_HH_DEFINE_DUMMY_GETTEXT
#endif

#endif
