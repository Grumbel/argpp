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

#ifndef HEADER_ARGPARSER_ITEM_HPP
#define HEADER_ARGPARSER_ITEM_HPP

#include <functional>
#include <string_view>
#include <vector>

#include "argument.hpp"

namespace argparser {

class Option;

struct OptionGroupItem
{
  std::string name;
  std::vector<std::unique_ptr<Option>> options;
};

struct TextItem
{
  std::string m_text;
};

struct CommandItem
{
  std::string name;
  std::unique_ptr<OptionGroupItem> option_group;
};

struct PositionalItem
{
  std::string name;
  std::function<void (std::string_view)> callback;
};

struct Option
{
  char short_name;
  std::string long_name;
  bool argument;
  std::function<void ()> callback_wo;
  std::function<void (std::string_view)> callback;
};

template<typename T>
struct TOption : public Option
{
public:
  /*
  TOption(char short_name, std::string long_name, Argument<T> argument) :
    m_short_name(short_name),
    m_long_name(std::move(long_name)),
    m_argument(std::move(argument))
  {}

  char short_name() const override;
  std::string const& long_name() const override;
  bool requires_argument() const override {
    return m_argument != nullptr;
  }

  void emit() override {
    assert(!requires_argument());
    callback();
  }

  void emit(std::string_view text) override {
    assert(requires_argument());
    m_callback(m_convert(text));
  }
  */

  char short_name;
  std::string long_name;
  std::unique_ptr<Argument<T>> argument;
  std::function<void (T)> callback;
};

} // namespace argparser

#endif

/* EOF */
