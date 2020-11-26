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

#include "argparser.hpp"
#include "option_group.hpp"
#include "prettyprinter.hpp"

namespace argparser {

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
  m_program(),
  m_usage(),
  m_root()
{
}

void
ArgParser::add_usage(std::string_view program, std::string_view usage)
{
  m_program = program;
  m_usage = usage;
}

OptionGroup&
ArgParser::add_command(std::string_view name)
{
  throw std::runtime_error("not implemented");
}

OptionGroup&
ArgParser::options()
{
  return m_root;
}

void
ArgParser::parse_args(int argc, char** argv)
{
  ParseContext ctx(argc, argv);

  OptionGroup* option_group = &m_root;

  int positional_counter = 0;
  while (ctx.next())
  {
    std::string_view const arg = ctx.arg();

    if (arg.empty() || arg[0] != '-') // rest
    {
      PositionalItem& item = option_group->lookup_positional(positional_counter);
      item.callback(arg);
      positional_counter += 1;
    }
    else if (arg[1] == '-') // --...
    {
      if (arg.size() == 2) // --
      {
        // Got a '--' treat everything after this as rest
        while (ctx.next()) {
          PositionalItem& item = option_group->lookup_positional(positional_counter);
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
        PositionalItem& item = option_group->lookup_positional(positional_counter);
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
  OptionGroup* option_group = &m_root;

  std::string_view opt = arg.substr(2);
  std::string::size_type const equal_pos = opt.find('=');
  if (equal_pos != std::string::npos) // --long-opt=with-arg
  {
    std::string_view opt_arg = opt.substr(equal_pos + 1);
    opt = opt.substr(0, equal_pos);

    Option& option = option_group->lookup_long_option(opt);
    if (option.requires_argument()) {
      dynamic_cast<OptionWithArg&>(option).call(opt_arg);
    } else {
      throw std::runtime_error("error doesn't need arg");
    }
  }
  else
  {
    Option& option = option_group->lookup_long_option(opt);
    if (!option.requires_argument()) {
      dynamic_cast<OptionWithoutArg&>(option).call();
    } else {
      if (!ctx.next()) {
        throw std::runtime_error("option '" + std::string(arg) + "' requires an argument");
      }
      dynamic_cast<OptionWithArg&>(option).call(ctx.arg());
    }
  }
}

void
ArgParser::parse_short_option(ParseContext& ctx, std::string_view arg)
{
  OptionGroup* option_group = &m_root;

  std::string_view const opts = arg.substr(1);

  for (size_t opts_i = 0; opts_i < opts.size(); ++opts_i) {
    char const opt = opts[opts_i];
    Option& option = option_group->lookup_short_option(opt);
    if (!option.requires_argument()) {
      dynamic_cast<OptionWithoutArg&>(option).call();
    } else {
      if (opts_i != opts.size() - 1) { // -fARG
        dynamic_cast<OptionWithArg&>(option).call(opts.substr(opts_i + 1));
        break;
      } else { // -f ARG
        if (!ctx.next()) {
          throw std::runtime_error("option '" + std::string(arg) + "' requires an argument");
        }
        dynamic_cast<OptionWithArg&>(option).call(ctx.arg());
      }
    }
  }
}

void
ArgParser::print_help(std::ostream& out) const
{
  const int terminal_width = get_terminal_width();
  const int column_min_width = 8;
  int column_width = column_min_width;

  { // Calculate left column width
    for (auto const& item : m_root.get_items())
    {
      if (Option* opt = dynamic_cast<Option*>(item.get())) {
        int width = 2; // add two leading space
        if (opt->get_short_name()) {
          width += 2; // "-a"
        }

        if (!opt->get_long_name().empty()) {
          width += static_cast<int>(opt->get_long_name().size()) + 2; // "--foobar"
        }

        if (opt->requires_argument()) {
          width += static_cast<int>(dynamic_cast<OptionWithArg&>(*opt).get_argument_name().size()) + 1;
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

  for (auto const& item : m_root.get_items())
  {
    if (TextItem const* text_item = dynamic_cast<TextItem*>(item.get())) {
      pprint.print(text_item->get_text());
    }
    // else if (opt.key == ArgumentType::PSEUDO)
    // {
    //   pprint.print(std::string(column_width, ' '), opt.long_name, opt.help);
    // }
    else if (Option* opt = dynamic_cast<Option*>(item.get())) {
      {
        constexpr size_t buffer_size = 256;
        std::array<char, buffer_size> option   = { 0 };
        std::array<char, buffer_size> argument = { 0 };

        if (opt->get_short_name())
        {
          if (opt->get_long_name().empty()) {
            snprintf(option.data(), option.size(), "-%c", opt->get_short_name());
          } else {
            snprintf(option.data(), option.size(), "-%c, --%s", opt->get_short_name(), opt->get_long_name().c_str());
          }
        }
        else
        {
          snprintf(option.data(), option.size(), "--%s", opt->get_long_name().c_str());
        }

        if (opt->requires_argument())
        {
          snprintf(argument.data(), argument.size(), " %s", dynamic_cast<OptionWithArg&>(*opt).get_argument_name().c_str());
        }

        std::string left_column("  ");
        left_column += option.data();
        left_column += argument.data();
        left_column += " ";

        pprint.print(std::string(column_width, ' '), left_column, opt->get_help());
      }
    }
  }
}

} // namespace argparser

/* EOF */
