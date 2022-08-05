%option c++
%option noyywrap
%option yylineno
%option case-insensitive

%{
	// implements the lexical scanner from
	// https://www.w3.org/TR/CSS2/grammar.html#scanner
	#include "token.h"
	using namespace css;
%}

h                                  [0-9a-f]
nonascii                           [\240-\377]
unicode                            \\{h}{1,6}(\r\n|[ \t\r\n\f])?
escape                             {unicode}|\\[^\r\n\f0-9a-f]
nmstart                            [_a-z]|{nonascii}|{escape}
nmchar                             [_a-z0-9-]|{nonascii}|{escape}
string1                            \"([^\n\r\f\\"]|\\{nl}|{escape})*\"
string2                            \'([^\n\r\f\\']|\\{nl}|{escape})*\'
badstring1                         \"([^\n\r\f\\"]|\\{nl}|{escape})*\\?
badstring2                         \'([^\n\r\f\\']|\\{nl}|{escape})*\\?
badcomment1                        \/\*[^*]*\*+([^/*][^*]*\*+)*
badcomment2                        \/\*[^*]*(\*+[^/*][^*]*)*
baduri1                            url\({w}([!#$%&*-\[\]-~]|{nonascii}|{escape})*{w}
baduri2                            url\({w}{string}{w}
baduri3                            url\({w}{badstring}
comment                            \/\*[^*]*\*+([^/*][^*]*\*+)*\/
ident                              -?{nmstart}{nmchar}*
name                               {nmchar}+
num                                [0-9]+|[0-9]*"."[0-9]+
string                             {string1}|{string2}
badstring                          {badstring1}|{badstring2}
badcomment                         {badcomment1}|{badcomment2}
baduri                             {baduri1}|{baduri2}|{baduri3}
url                                ([!#$%&*-~]|{nonascii}|{escape})*
s                                  [ \t\r\n\f]+
w                                  {s}?
nl                                 \n|\r\n|\r|\f

%%

{s}                                { /* whitespace */ }

\/\*[^*]*\*+([^/*][^*]*\*+)*\/     { /* comments */ }
{badcomment}                       { /* unclosed comment at EOF */ }

"<!--"                             { return CDO; }
"-->"                              { return CDC; }
"~="                               { return INCLUDES; }
"|="                               { return DASHMATCH; }

{string}                           { return STRING; }
{badstring}                        { return BAD_STRING; }
{ident}			                   { return IDENT; }

"#"{name}                          { return HASH; }

"@import"                          { return IMPORT_SYM; }
"@page"                            { return PAGE_SYM; }
"@media"                           { return MEDIA_SYM; }
"@charset "		                   { return CHARSET_SYM; }

"!"({w}|{comment})*"important"     { return IMPORTANT_SYM; }

{num}"em"                          { return EMS; }
{num}"ex"                          { return EXS; }
{num}"px"                          { return LENGTH; }
{num}"cm"                          { return LENGTH; }
{num}"mm"                          { return LENGTH; }
{num}"in"                          { return LENGTH; }
{num}"pt"                          { return LENGTH; }
{num}"pc"                          { return LENGTH; }
{num}"deg"                         { return ANGLE; }
{num}"rad"                         { return ANGLE; }
{num}"grad"                        { return ANGLE; }
{num}"ms"                          { return TIME; }
{num}"s"                           { return TIME; }
{num}"hz"                          { return FREQ; }
{num}"khz"                         { return FREQ; }
{num}{ident}                       { return DIMENSION; }

{num}%                             { return PERCENTAGE; }
{num}                              { return NUMBER; }

"url("{w}{string}{w}")"            { return URI; }
"url("{w}{url}{w}")"               { return URI; }
{baduri}                           { return BAD_URI; }

{ident}"("                         { return FUNCTION; }

.                                  { return *yytext; }

%%
