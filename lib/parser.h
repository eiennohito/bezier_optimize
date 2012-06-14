#ifndef parser_h__4dba348b_5979_4008_bb09_752cef54fc80
#define parser_h__4dba348b_5979_4008_bb09_752cef54fc80
#include <string>
#include <vector>
#include "BezierFragment.h"

bool parse_string(const std::string& val, std::vector<BezierFragment>& frags);
bool parse_strings(const std::string& val, std::vector<std::vector<BezierFragment> >& frags);
#endif // parser_h__4dba348b_5979_4008_bb09_752cef54fc80