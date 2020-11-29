// ArgParser
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

#include <filesystem>
#include <iostream>
#include <optional>

#include <argparser/argparser.hpp>

int main(int argc, char** argv)
{
  try
  {
    int verbose = 0;

    using argparser::Argument;
    argparser::ArgParser argp;

    argp.add_program(argv[0]);

    argp.add_text("Short description of what it does");
    argp.add_newline();
    argp.add_text("Lengthy description of what the program does. "
                  "Lengthy description of what the program does. "
                  "Lengthy description of what the program does. "
                  "Lengthy description of what the program does. "
                  "Lengthy description of what the program does. "
                  "Lengthy description of what the program does. ");

    argp.add_group("Options:");
    argp.add_option('v', "version", "Version");
    argp.add_option('V', "verbose", "Version").increment(verbose);
    argp.add_option('h', "help", "Help text").then([&]{ argp.print_help(); });
    argp.add_alias('H', argp.lookup_short_option('h'));
    argp.add_alias("hilfe", argp.lookup_long_option("help"));
    argp.add_option({}, "long-only", Argument("ARG"), "Blabla");

    //argp.add_option('c', "complex", Argument<int,int,int>("X:Y:Z"), "Blabla");

    std::optional<std::string> stringvar;
    argp.add_option('X', {}, Argument("ARG"), "Blabla").store(stringvar);

    argp.add_group("Options with arguments:");
    argp.add_option('f', "file", Argument<std::filesystem::path>("FILE"), "Do File").then([](auto const& path) {
      std::cout << "Got path: " << path << std::endl;
    });
    argp.add_option('n', "number", Argument<int>("FILE"), "Number").then([](int number) {
      std::cout << "Got int: " << number << std::endl;
    });

    argp.add_option('t', "text", Argument("FILE"), "Number").then([](std::string_view text) {
      std::cout << "Got text: " << text << std::endl;
    });

    //auto& group = argp.add_group("Display Options:");
    //gcmd = add_command("install");
    //group.add_option("+v", "--version", "", "Help text");
    //group.add_option({"+v", "+version", Argument::NONE, "Help text"}).then([]{ .... });
    //group.add_option("-v", "-version", "Help text").store(&var, true);
    //group.add_option("-v", "-version", "Help text").store(&var, true);
    int var = 0;
    argp.add_option('s', "store", "Help text").store(var, 5);
    argp.add_option('S', "no-store", "Help text").store(var, -10);
    //group.add_option("-v", "-version", Argument<int>("FILE"), "Help text").append(&var);
    //group.add_option("-v", "-version", Argument<int>("FILE"), "Help text").then([](ParseContext& ctx, int value){
    //ctx.raise_exception("illegal option");
    //});

    //group.add(Option("-v", "--version").argument(Argument::NONE).help("Help text").store(&var, false));
    //Argument<std::tuple<int, int, std::string>>();
    //Argument<std::vector<int>()

    argp.add_group("Commands:");
    auto& install_cmd = argp.add_command("install", "Install stuff");
    auto& install_opts = install_cmd.get_options();
    install_opts.add_text("Install stuff");

    install_opts.add_group("Options:");
    install_opts.add_option('h', "help", "Help text").then([&]{ argp.print_help(install_cmd); });
    install_opts.add_option('n', "number", "Blabla");
    install_opts.add_option('d', "device", "Blabla");

    install_opts.add_group("Positional arguments:");
    install_opts.add_positional(Argument("PACKAGE"), "Package to install").then([](std::string_view text){
      std::cout << "Installing package: " << text << "\n";
    });

    auto& search_cmd = argp.add_command("search", "Search stuff");
    auto& search_opts = search_cmd.get_options();
    search_opts.add_option('h', "help", "Help text").then([&]{ argp.print_help(search_cmd); });
    search_opts.add_option('n', "number", "Blabla");
    search_opts.add_option('d', "device", "Blabla");

    search_opts.add_group("Positonal Arguments:");
    search_opts.add_positional(Argument("FLUB"), "File to load");
    search_opts.add_positional(Argument("BLOB"), "Output file");

    search_opts.add_group("Rest Arguments:");
    search_opts.add_rest(Argument<std::filesystem::path>("FILE"), "Files to do stuff with");

    /*
    argp.add_newline();
    argp.add_group("Positional Arguments:");
    argp.add_positional(Argument("FLUB"), "File to load").then([](std::string_view text){
      std::cout << "flub: " << text << std::endl;
    });
    argp.add_positional(Argument<std::filesystem::path>("BLOB"), "Output file").then([](std::filesystem::path const& text){
      std::cout << "blob: " << text << std::endl;
    });
    argp.add_positional(Argument<int>("NUMBER"), "a number file").then([](int number){
      std::cout << "number: " << number << std::endl;
    });
    */

    argp.add_newline();
    argp.add_text("Copyright, author email and all that stuff");

    std::cout << "----------------------------------------------" << std::endl;
    argp.parse_args(argc, argv);

    std::cout << "Store: " << var << std::endl;
    std::cout << "Verbose: " << verbose << std::endl;
    std::cout << "StringVar: " << (stringvar ? *stringvar : "<not set>") << std::endl;

    return 0;
  }
  catch(const std::exception& err)
  {
    std::cout << "Error: " << err.what() << std::endl;
    return 1;
  }
}

/* EOF */
