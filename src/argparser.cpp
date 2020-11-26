// ArgParse - A Command Line Argument Parser for C++
// Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <sys/ioctl.h>

#include <span>
#include <string_view>

#include <argparser/argparser.hpp>

namespace argparser {

#if 0
namespace {

constexpr int max_column_width = 5;
constexpr int default_terminal_width = 80;

int get_terminal_width()
{
  struct winsize w;
  if (ioctl(0, TIOCGWINSZ, &w) < 0)
  {
    return default_terminal_width;
  }
  else
  {
    return w.ws_col;
  }
}

} // namespace
#endif

void
OptionGroup::add_text(std::string_view text)
{
}

void
OptionGroup::add_newline()
{
}

Option&
OptionGroup::add_option(std::unique_ptr<Option> option)
{
  return *option;
}

void
OptionGroup::add_alias(std::string alias, std::string_view name)
{
}

void
OptionGroup::add_alias(char alias, char name)
{
}

class ParseContext
{
public:
  ParseContext(int argc, char** argv) :
    m_idx(0),
    m_argv(argv, argc)
  {}

  std::string_view arg() const {
    return m_argv[m_idx];
  }

  bool next() {
    if (m_idx < m_argv.size() - 1) {
      m_idx += 1;
      return true;
    } else {
      return false;
    }
  }

  std::string_view program() const { return m_argv[0]; }

private:
  size_t m_idx;
  std::span<char*> m_argv;
};

ArgParser::ArgParser() :
  m_items()
{
}

void
ArgParser::add_usage(std::string_view program, std::string_view usage)
{
}

OptionGroup&
ArgParser::add_group(std::string_view name)
{
  return *(new OptionGroup);
}

OptionGroup&
ArgParser::add_command(std::string_view name)
{
  return *(new OptionGroup);
}

PositionalItem&
ArgParser::lookup_positional(int i)
{
  throw std::runtime_error("not implemented");
}

Option&
ArgParser::lookup_short_option(char c)
{
  throw std::runtime_error("not implemented");
}

Option&
ArgParser::lookup_long_option(std::string_view)
{
  throw std::runtime_error("not implemented");
}

void
ArgParser::parse_args(int argc, char** argv)
{
  ParseContext ctx(argc, argv);

  int positional_counter = 0;
  while (ctx.next())
  {
    std::string_view const arg = ctx.arg();

    if (arg.empty() || arg[0] != '-') // rest
    {
      PositionalItem& item = lookup_positional(positional_counter);
      item.callback(arg);
      positional_counter += 1;
    }
    else if (arg[1] == '-') // --...
    {
      if (arg.size() == 2) // --
      {
        // Got a '--' treat everything after this as rest
        while (ctx.next()) {
          PositionalItem& item = lookup_positional(positional_counter);
          item.callback(arg);
          positional_counter += 1;
        }
        break;
      }
      else
      {
        parse_long_option(ctx, arg);
      }
    }
    else // short option
    {
      if (arg.size() == 1) // -
      {
        PositionalItem& item = lookup_positional(positional_counter);
        item.callback(arg);
        positional_counter += 1;
      }
      else
      {
        parse_short_option(ctx, arg);
      }
    }
  }
}

void
ArgParser::parse_long_option(ParseContext& ctx, std::string_view arg)
{
  std::string_view opt = arg.substr(2);
  std::string::size_type const equal_pos = arg.find('=', 2);
  if (equal_pos != std::string::npos) // --long-opt=with-arg
  {
    opt = arg.substr(2, equal_pos);
    std::string_view opt_arg = arg.substr(equal_pos + 1);

    Option& option = lookup_long_option(opt);
    if (option.argument) {
      option.callback(opt_arg);
    } else {
      // FIXME: error doesn't need arg
    }
  }
  else
  {
    Option& option = lookup_long_option(opt);
    if (!option.argument) {
      option.callback(arg);
    } else {
      if (!ctx.next()) {
        throw std::runtime_error("option '" + std::string(arg) + "' requires an argument");
      }
      option.callback(ctx.arg());
    }
  }
}

void
ArgParser::parse_short_option(ParseContext& ctx, std::string_view arg)
{
  std::string_view const opts = arg.substr(1);

  for (size_t opts_i = 0; opts_i < opts.size(); ++opts_i) {
    char const opt = opts[opts_i];
    Option& option = lookup_short_option(opt);
    if (!option.argument) {
      option.callback_wo();
    } else {
      if (opts_i != opts.size() - 1) { // -fARG
        option.callback(opts.substr(opts_i));
        break;
      } else { // -f ARG
        if (!ctx.next()) {
          throw std::runtime_error("option '" + std::string(arg) + "' requires an argument");
        }
        option.callback(ctx.arg());
      }
    }
  }
}

void
ArgParser::print_help(std::ostream& out) const
{
#if 0
  const int terminal_width = get_terminal_width();
  const int column_min_width = 8;
  int column_width = column_min_width;

  { // Calculate left column width
    for(auto const& group : m_groups)
    {
      for(auto const& opt : group.m_options)
      {
        int width = 2; // add two leading space
        if (opt.short_option) {
          width += 2; // "-a"
        }

        if (!opt.long_option.empty()) {
          width += static_cast<int>(opt.long_option.size()) + 2; // "--foobar"
        }

        if (!opt.argument.empty()) {
          width += static_cast<int>(opt.argument.size()) + 1;
        }

        column_width = std::max(column_width, width);
      }
    }

    column_width = column_width+2; // add two trailing space
  }

  if (terminal_width < column_width * 3)
  {
    column_width -= (column_width*3 - terminal_width);
    column_width = std::max(column_width, max_column_width);
  }

  PrettyPrinter pprint(terminal_width); // -1 so we have a whitespace on the right side

  out << "Usage: " << m_program << " " <<  m_usage << '\n';

  if (!m_groups.empty()) {
    out << '\n';
  }

  for(auto const& group : m_groups)
  {
    for(auto const& opt : group.m_options)
    {
      if (opt.visible)
      {
        if (opt.key == ArgumentType::TEXT)
        {
          pprint.print(opt.help);
        }
        else if (opt.key == ArgumentType::PSEUDO)
        {
          pprint.print(std::string(column_width, ' '), opt.long_option, opt.help);
        }
        else
        {
          constexpr size_t buffer_size = 256;
          std::array<char, buffer_size> option   = { 0 };
          std::array<char, buffer_size> argument = { 0 };

          if (opt.short_option)
          {
            if (opt.long_option.empty()) {
              snprintf(option.data(), option.size(), "-%c", opt.short_option);
            } else {
              snprintf(option.data(), option.size(), "-%c, --%s", opt.short_option, opt.long_option.c_str());
            }
          }
          else
          {
            snprintf(option.data(), option.size(), "--%s", opt.long_option.c_str());
          }

          if (!opt.argument.empty())
          {
            snprintf(argument.data(), argument.size(), " %s", opt.argument.c_str());
          }

          std::string left_column("  ");
          left_column += option.data();
          left_column += argument.data();
          left_column += " ";

          pprint.print(std::string(column_width, ' '), left_column, opt.help);
        }
      }
    }

    if (&group != &m_groups.back())
    {
      std::cout << '\n';
    }
  }
#endif
}

} // namespace argparser

/* EOF */
