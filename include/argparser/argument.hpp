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

#ifndef HEADER_ARGPARSER_ARGUMENT_HPP
#define HEADER_ARGPARSER_ARGUMENT_HPP

#include <string_view>

namespace argparser {

template<typename F>
struct Argument
{
  //std::result_of<F()>::type convert(std::string_text text) {
  //  return convert_func(text);
  //}

  std::string name;
  F convert_func;
};

} // namespace argparser

#endif

/* EOF */
