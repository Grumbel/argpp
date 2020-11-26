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

#ifndef HEADER_ARGPARSER_ITEM_HPP
#define HEADER_ARGPARSER_ITEM_HPP

#include <assert.h>
#include <functional>
#include <string_view>
#include <variant>
#include <vector>

#include "argument.hpp"

namespace argparser {

class Option;
class OptionGroup;

class Item
{
public:
  virtual ~Item() {}
};

class TextItem : public Item
{
public:
  TextItem(std::string text) :
    m_text(std::move(text))
  {}

  std::string const& get_text() const { return m_text; }

private:
  std::string m_text;
};

class CommandItem : public Item
{
public:
  std::string name;
  std::unique_ptr<OptionGroup> option_group;
};

class PositionalItem : public Item
{
public:
  std::string name;
  std::function<void (std::string_view)> callback;
};

class RestItem : public Item
{
public:
  std::string name;
  std::function<void (std::string_view)> callback;
};

class LongOptionAlias : public Item
{
public:
  LongOptionAlias(std::string name, Option& option) :
    m_name(name),
    m_option(option)
  {}

  std::string const& get_name() const { return m_name; }
  Option& get_option() const { return m_option; }

private:
  std::string m_name;
  Option& m_option;
};

class ShortOptionAlias : public Item
{
public:
  ShortOptionAlias(char name, Option& option) :
    m_name(name),
    m_option(option)
  {}

  char get_name() const { return m_name; }
  Option& get_option() const { return m_option; }

private:
  char m_name;
  Option& m_option;
};

} // namespace argparser

#endif

/* EOF */
