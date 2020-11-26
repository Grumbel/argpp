#include "parser.hpp"

namespace argparse {

Parser::Parser(Grammar const& grammar) :
  m_grammar(grammar)
{
}

// Option const* option = lookup_long_option(long_opt);
// if (!option) {
//   throw std::runtime_error("unrecognized option '" + std::string(arg) + "'");
// }

// if (!option) {
//     throw std::runtime_error("invalid option -- " + std::string(1, *p));
//   }

// Option const* option = lookup_short_option(*p);
// if (!option->requires_argument()) {
//   option->callback(ctx);
// }
// else
// {
//   if (i == argc - 1 || *(p+1) != '\0') {
//     throw std::runtime_error("option requires an argument -- " + std::string(1, *p));
//   }
//   parsed_options.push_back(ParsedOption{option->key, std::string(1, *p), argv[i + 1]});
//   ++i;
// }
//    ++p;
//       }
//     }
//   }
// }
// else
// {
//   parsed_options.push_back(ParsedOption{ArgumentType::REST, "", arg});
// }

} // namespace argparse

/* EOF */
