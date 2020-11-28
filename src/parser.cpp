// ArgParse - A Command Line Argument Parser for C++
// Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
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

#include <fmt/format.h>

#include <span>
#include <string_view>

#include "command_item.hpp"
#include "error.hpp"
#include "option_group.hpp"
#include "parser.hpp"
#include "positional_item.hpp"
#include "prettyprinter.hpp"
#include "text_item.hpp"

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
    m_argv(argv, argc),
    m_positional_counter(0)
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

  void incr_positional_counter() { m_positional_counter += 1; }
  int get_positional_counter() const { return m_positional_counter; }

private:
  size_t m_idx;
  std::span<char*> m_argv;
  int m_positional_counter;
};

Parser::Parser() :
  m_program()
{
}

void
Parser::add_program(std::string_view program)
{
  m_program = program;
}

void
Parser::parse_args(int argc, char** argv)
{
  ParseContext ctx(argc, argv);
  parse_args(ctx, *this);
}

void
Parser::parse_args(ParseContext& ctx, OptionGroup& group)
{
  while (ctx.next())
  {
    std::string_view const arg = ctx.arg();

    if (arg.empty() || arg[0] != '-') // rest
    {
      parse_non_option(ctx, group, arg);
    }
    else if (arg[1] == '-') // --...
    {
      if (arg.size() == 2) { // --
        // Got a '--' treat everything after this as rest
        while (ctx.next()) {
          parse_non_option(ctx, group, arg);
        }
        break;
      } else {
        parse_long_option(ctx, group, arg);
      }
    }
    else // short option
    {
      if (arg.size() == 1) { // -
        parse_non_option(ctx, group, arg);
      } else {
        parse_short_option(ctx, group, arg);
      }
    }
  }

  // FIXME:
  // - check if all required option where given
  // - add mutual exclusion checks
}

void
Parser::parse_non_option(ParseContext& ctx, OptionGroup& group, std::string_view arg)
{
  if (group.has_commands()) {
    CommandItem& command_item = group.lookup_command(arg);
    parse_args(ctx, command_item.get_options());
  } else {
    if (group.has_positional()) {
      try {
        PositionalItem& item = group.lookup_positional(ctx.get_positional_counter());
        item.call(arg); // FIXME: throw something useful when the conversion fails
        ctx.incr_positional_counter();
      } catch (...) {
        throw Error(fmt::format("unknown item in position {}: {}", ctx.get_positional_counter(), arg));
      }
    }
    //if (group.has_positional()) {
  }
}

void
Parser::parse_long_option(ParseContext& ctx, OptionGroup& group, std::string_view arg)
{
  std::string_view opt = arg.substr(2);
  std::string::size_type const equal_pos = opt.find('=');
  if (equal_pos != std::string::npos) // --long-opt=with-arg
  {
    std::string_view opt_arg = opt.substr(equal_pos + 1);
    opt = opt.substr(0, equal_pos);

    Option const& option = group.lookup_long_option(opt);
    if (option.requires_argument()) {
      dynamic_cast<OptionWithArg const&>(option).call(opt_arg);
    } else {
      throw Error("error doesn't need arg");
    }
  }
  else
  {
    Option const& option = group.lookup_long_option(opt);
    if (!option.requires_argument()) {
      dynamic_cast<OptionWithoutArg const&>(option).call();
    } else {
      if (!ctx.next()) {
        throw Error("option '" + std::string(arg) + "' requires an argument");
      }
      dynamic_cast<OptionWithArg const&>(option).call(ctx.arg());
    }
  }
}

void
Parser::parse_short_option(ParseContext& ctx, OptionGroup& group, std::string_view arg)
{
  std::string_view const opts = arg.substr(1);

  for (size_t opts_i = 0; opts_i < opts.size(); ++opts_i) {
    char const opt = opts[opts_i];
    Option const& option = group.lookup_short_option(opt);
    if (!option.requires_argument()) {
      dynamic_cast<OptionWithoutArg const&>(option).call();
    } else {
      if (opts_i != opts.size() - 1) { // -fARG
        dynamic_cast<OptionWithArg const&>(option).call(opts.substr(opts_i + 1));
        break;
      } else { // -f ARG
        if (!ctx.next()) {
          throw Error("option '" + std::string(arg) + "' requires an argument");
        }
        dynamic_cast<OptionWithArg const&>(option).call(ctx.arg());
      }
    }
  }
}

void
Parser::print_usage(CommandItem const* current_command_item, std::ostream& out) const
{
  auto print_group = [&](OptionGroup const& group) {
    if (group.has_options()) {
      out << " [OPTION]...";
    }

    for (auto const& item : group.get_items()) {
      if (auto* positional_item = dynamic_cast<PositionalItem*>(item.get())) {
        out << " " << positional_item->get_name();
      }
    }

    for (auto const& item : group.get_items()) {
      if (auto* rest_item = dynamic_cast<RestItem*>(item.get())) {
        out << " " << rest_item->get_name();
      }
    }
  };

  if (!has_commands())
  {
    out << "Usage: " << m_program;
    print_group(*this);
  }
  else
  {
    size_t item_idx = 0;
    for (auto const& item : get_items()) {
      if (auto* command_item = dynamic_cast<CommandItem*>(item.get())) {
        if (current_command_item != nullptr &&
            current_command_item != command_item) {
          continue;
        }

        if (item_idx == 0) {
          out << "Usage: ";
        } else {
          out << "\n       ";
        }

        out << m_program;

        if (has_options()) {
          out << " [OPTION]...";
        }

        out << " " << command_item->get_name();

        print_group(command_item->get_options());

        item_idx += 1;
      }
    }
  }

  out << std::endl;
}

void
Parser::print_help(CommandItem const& command_item, std::ostream& out) const
{
  print_help(command_item.get_options(), &command_item, out);
}

void
Parser::print_help(OptionGroup const& group, CommandItem const* current_command_item, std::ostream& out) const
{
  const int terminal_width = std::min(get_terminal_width(), 120);
  const int column_min_width = 8;
  int column_width = column_min_width;

  { // Calculate left column width
    for (auto const& item : group.get_items())
    {
      if (auto* opt = dynamic_cast<Option*>(item.get())) {
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

  print_usage(current_command_item, out);

  for (auto const& item : group.get_items())
  {
    if (auto const* text_item = dynamic_cast<TextItem*>(item.get())) {
      pprint.print(text_item->get_text());
    } else if (auto* pseudo_item = dynamic_cast<PseudoItem*>(item.get())) {
      pprint.print(std::string(column_width, ' '), pseudo_item->get_name(), pseudo_item->get_help());
    } else if (auto* positional_item = dynamic_cast<PositionalItem*>(item.get())) {
      pprint.print(std::string(column_width, ' '), "  " + positional_item->get_name(), positional_item->get_help());
    } else if (auto* rest_item = dynamic_cast<RestItem*>(item.get())) {
      pprint.print(std::string(column_width, ' '), "  " + rest_item->get_name(), rest_item->get_help());
    } else if (auto* opt = dynamic_cast<Option*>(item.get())) {
      constexpr size_t buffer_size = 256;
      std::array<char, buffer_size> option   = { 0 };
      std::array<char, buffer_size> argument = { 0 };

      if (opt->get_short_name()) {
        if (opt->get_long_name().empty()) {
          snprintf(option.data(), option.size(), "-%c", opt->get_short_name());
        } else {
          snprintf(option.data(), option.size(), "-%c, --%s", opt->get_short_name(), opt->get_long_name().c_str());
        }
      } else {
        snprintf(option.data(), option.size(), "--%s", opt->get_long_name().c_str());
      }

      if (opt->requires_argument()) {
        snprintf(argument.data(), argument.size(), "%c%s",
                 !opt->get_long_name().empty() ? '=' : ' ',
                 dynamic_cast<OptionWithArg&>(*opt).get_argument_name().c_str());
      }

      std::string left_column("  ");
      left_column += option.data();
      left_column += argument.data();
      left_column += " ";

      pprint.print(std::string(column_width, ' '), left_column, opt->get_help());
    } else if (auto* command_item = dynamic_cast<CommandItem*>(item.get())) {
      pprint.print(std::string(column_width, ' '), "  " + command_item->get_name(), command_item->get_help());
    } else {
      // unhandled items
    }
  }
}

void
Parser::print_help(std::ostream& out) const
{
  print_help(*this, nullptr, out);
}

} // namespace argparser

/* EOF */
