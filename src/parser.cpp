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
  ParseContext(std::span<char const* const> argv) :
    m_idx(0),
    m_argv(argv),
    m_positional_index(0),
    m_mutual_exclusion_state(0u)
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

  void incr_positional_index() { m_positional_index += 1; }
  int get_positional_index() const { return m_positional_index; }

  void add_mutual_exclusion_state(uint32_t mask) { m_mutual_exclusion_state |= mask; }
  uint32_t get_mutual_exclusion_state() const { return m_mutual_exclusion_state; }

private:
  size_t m_idx;
  std::span<char const* const> m_argv;
  int m_positional_index;
  uint32_t m_mutual_exclusion_state;
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
Parser::parse_args(int argc, char const* const* argv)
{
  parse_args(std::span<char const* const>(argv, argc));
}

void
Parser::parse_args(std::span<char const* const> argv)
{
  ParseContext ctx(argv);
  parse_args(ctx, *this);
}

void
Parser::parse_args(ParseContext& ctx, OptionGroup const& group)
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
Parser::parse_non_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg)
{
  if (group.has_commands()) {
    CommandItem const& command_item = group.lookup_command(arg);
    check_mutual_exclusion(ctx, command_item);
    parse_args(ctx, command_item.get_options());
  } else {
    if (ctx.get_positional_index() < group.get_positional_count() ) {
      PositionalItem const& item = group.lookup_positional(ctx.get_positional_index());
      check_mutual_exclusion(ctx, item);
      try {
        item.call(arg);
      } catch (std::exception const& err) {
        throw Error(fmt::format("invalid positional item at {}: {}: {}", ctx.get_positional_index(), arg, err.what()));
      } catch (...) {
        std::throw_with_nested(Error(fmt::format("invalid positional item at position {}: {}", ctx.get_positional_index(), arg)));
      }
      ctx.incr_positional_index();
    } else if (group.has_rest()) {
      RestItem const& item = group.lookup_rest();
      check_mutual_exclusion(ctx, item);
      try {
        item.call(arg);
      } catch (std::exception const& err) {
        throw Error(fmt::format("invalid rest item at {}: {}: {}", ctx.get_positional_index(), arg, err.what()));
      } catch (...) {
        std::throw_with_nested(Error(fmt::format("invalid rest item at {}: {}", ctx.get_positional_index(), arg)));
      }
    } else {
      throw Error(fmt::format("unknown item in position {}: {}", ctx.get_positional_index(), arg));
    }
  }
}

void
Parser::parse_long_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg)
{
  std::string_view opt = arg.substr(2);
  std::string::size_type const equal_pos = opt.find('=');
  if (equal_pos != std::string::npos) // --long-opt=with-arg
  {
    std::string_view opt_arg = opt.substr(equal_pos + 1);
    opt = opt.substr(0, equal_pos);

    Option const& option = group.lookup_long_option(opt);
    check_mutual_exclusion(ctx, option);
    if (option.requires_argument()) {
      dynamic_cast<OptionWithArg const&>(option).call(opt_arg);
    } else {
      throw Error("error doesn't need arg");
    }
  }
  else
  {
    Option const& option = group.lookup_long_option(opt);
    check_mutual_exclusion(ctx, option);
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
Parser::parse_short_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg)
{
  std::string_view const opts = arg.substr(1);

  for (size_t opts_i = 0; opts_i < opts.size(); ++opts_i) {
    char const opt = opts[opts_i];
    Option const& option = group.lookup_short_option(opt);
    check_mutual_exclusion(ctx, option);
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
Parser::print_usage(std::ostream& out) const
{
  print_usage(nullptr, out);
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
        if (positional_item->get_flags().is_required()) {
          out << " " << positional_item->get_name();
        } else {
          out << " [" << positional_item->get_name() << "]";
        }
      }
    }

    for (auto const& item : group.get_items()) {
      if (auto* rest_item = dynamic_cast<RestItem*>(item.get())) {
        if (rest_item->get_flags().is_required()) {
          out << " " << rest_item->get_name() << "...";
        } else {
          out << " [" << rest_item->get_name() << "]...";
        }
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
Parser::print_help(CommandItem const& command_item, std::ostream& out, uint32_t visibility_mask) const
{
  print_help(command_item.get_options(), &command_item, out, visibility_mask);
}

void
Parser::print_help(OptionGroup const& group, CommandItem const* current_command_item, std::ostream& out, uint32_t visibility_mask) const
{
  // FIXME: move this into PrettyPrinter
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

        if (opt->get_long_name()) {
          width += static_cast<int>(opt->get_long_name()->size()) + 2; // "--foobar"
        }

        if (opt->get_short_name() && opt->get_long_name()) {
          width += 2; // ", "
        }

        if (opt->requires_argument()) {
          width += static_cast<int>(dynamic_cast<OptionWithArg&>(*opt).get_argument_name().size()) + 1; // "=ARG"
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

  PrettyPrinter pprinter(terminal_width); // -1 so we have a whitespace on the right side

  pprinter.set_column_width(column_width);
  print_usage(current_command_item, out);

  for (auto const& item : group.get_items()) {
    if (item->get_flags().get_visibility() & visibility_mask) {
      item->print(pprinter);
    }
  }
}

void
Parser::print_help(std::ostream& out, uint32_t visibility_mask) const
{
  print_help(*this, nullptr, out, visibility_mask);
}

void
Parser::check_mutual_exclusion(ParseContext& ctx, Item const& item)
{
  if (ctx.get_mutual_exclusion_state() & item.get_flags().get_mutual_exclusion()) {
    throw Error("mutual exclusion violation"); // FIXME: add better error message
  } else {
    ctx.add_mutual_exclusion_state(item.get_flags().get_mutual_exclusion());
  }
}

} // namespace argparser

/* EOF */
