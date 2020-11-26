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
  std::string text;
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

class Option : public Item
{
public:
  Option(char short_name_, std::string long_name_, std::unique_ptr<ArgumentBase> argument, std::string help_) :
    short_name(short_name_),
    long_name(long_name_),
    m_argument(std::move(argument)),
    m_help(help_),
    m_callback()
  {}

  std::string const& get_help() const { return m_help; }

  bool requires_argument() const { return m_argument != nullptr; }
  std::string const& get_argument_name() const {
    assert(m_argument != nullptr);
    return m_argument->get_name();
  }

  void on(std::function<void ()> cb) {
    assert(!requires_argument());

    m_callback = cb;
  }

  template<typename T>
  void on_arg2(std::function<void (T)> cb) {
    assert(requires_argument());

    Argument<T>* arg = dynamic_cast<Argument<T>*>(m_argument.get());
    assert(arg != nullptr);
    arg->on(cb);
  }

  template<typename T>
  void store(T& place, T const value) {
    assert(!requires_argument());

    on([&place, value = std::move(value)]{
      place = value;
    });
  }

  void call() {
    m_callback();
  }

  void call(std::string_view text) {
    m_argument->call(text);
  }

  char short_name;
  std::string long_name;

private:
  std::unique_ptr<ArgumentBase> m_argument;
  std::string m_help;
  std::function<void ()> m_callback;
};

#if 0
template<typename T>
class TOption : public Option
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
#endif

class LongOptionAlias : public Item
{
public:
  LongOptionAlias(std::string name, Option& option) :
    m_name(name),
    m_option(option)
  {}

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

  char m_name;
  Option& m_option;
};

} // namespace argparser

#endif

/* EOF */
