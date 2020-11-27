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

#include "option_group.hpp"

#include <fmt/format.h>

#include "alias_item.hpp"
#include "command_item.hpp"
#include "positional_item.hpp"
#include "text_item.hpp"

namespace argparser {

void
OptionGroup::add_group(std::string name)
{
  m_items.push_back(std::make_unique<TextItem>(std::move(name)));
}

void
OptionGroup::add_text(std::string text)
{
  m_items.push_back(std::make_unique<TextItem>(std::move(text)));
}

void
OptionGroup::add_newline()
{
  m_items.push_back(std::make_unique<TextItem>(""));
}

void
OptionGroup::add_pseudo(std::string name, std::string help)
{
  m_items.push_back(std::make_unique<PseudoItem>(std::move(name), std::move(help)));
}

void
OptionGroup::add_rest(std::string name)
{
  m_items.push_back(std::make_unique<RestItem>(std::move(name)));
}

Option&
OptionGroup::add_option(std::unique_ptr<Option> option)
{
  Option& option_ref = *option;
  m_items.push_back(std::move(option));
  return option_ref;
}

OptionGroup&
OptionGroup::add_command(std::string name, std::string help)
{
  auto command_item = std::make_unique<CommandItem>(std::move(name), std::move(help));
  OptionGroup& option_group = command_item->get_options();
  m_items.push_back(std::move(command_item));
  return option_group;
}

void
OptionGroup::add_alias(std::string name, Option& option)
{
  m_items.push_back(std::make_unique<LongOptionAlias>(std::move(name), option));
}

void
OptionGroup::add_alias(char name, Option& option)
{
  m_items.push_back(std::make_unique<ShortOptionAlias>(name, option));
}

CommandItem&
OptionGroup::lookup_command(std::string_view name)
{
  for (auto& item : m_items) {
    Item* ptr = item.get();
    if (auto* command_item = dynamic_cast<CommandItem*>(ptr)) {
      if (command_item->get_name() == name) {
        return *command_item;
      }
    }
  }
  throw std::runtime_error(fmt::format("command item '{}' not found", name));
}

PositionalItem&
OptionGroup::lookup_positional(int i)
{
  int positional_count = 0;
  for (auto& item : m_items) {
    Item* ptr = item.get();
    if (auto* positional_item = dynamic_cast<PositionalItem*>(ptr)) {
      if (positional_count == i) {
        return *positional_item;
      }
      positional_count += 1;
    }
  }
  throw std::runtime_error(fmt::format("positional item {} not found", i));
}

Option&
OptionGroup::lookup_short_option(char name)
{
  for (auto& item : m_items) {
    if (auto* option = dynamic_cast<Option*>(item.get())) {
      if (option->get_short_name() == name) {
        return *option;
      }
    } else if (auto* alias = dynamic_cast<ShortOptionAlias*>(item.get());
               alias && alias->get_name() == name) {
      return alias->get_option();
    }
  }
  throw std::runtime_error(fmt::format("short option '{}' not found", name));
}

Option&
OptionGroup::lookup_long_option(std::string_view name)
{
  for (auto& item : m_items) {
    if (auto* option = dynamic_cast<Option*>(item.get())) {
      if (option->get_long_name() == name) {
        return *option;
      }
    } else if (auto* alias = dynamic_cast<LongOptionAlias*>(item.get());
               alias && alias->get_name() == name) {
      return alias->get_option();
    }
  }
  throw std::runtime_error(fmt::format("long option '{}' not found", name));
}

} // namespace argparser

/* EOF */
