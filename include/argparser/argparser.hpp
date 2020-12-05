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

#ifndef HEADER_ARGPARSER_ARGPARSER_HPP
#define HEADER_ARGPARSER_ARGPARSER_HPP

#include <span>

#include "argument.hpp"
#include "command_item.hpp"
#include "flags.hpp"
#include "option_group.hpp"
#include "parser.hpp"
#include "printer.hpp"
#include "rest_options_item.hpp"

namespace argparser {

using ArgParser = Parser;

void parse(OptionGroup const& options, int argc, char const* const* argv);
void parse(OptionGroup const& options, std::span<char const* const> argv);

} // namespace argparser

#endif

/* EOF */
