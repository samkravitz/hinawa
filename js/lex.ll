%option c++
%option noyywrap
%option yylineno
%option case-insensitive
%option prefix="js"

%{
	#include <iostream>
	#include <string>

	#include "token_type.h"
	using namespace js;
%}

%%

%{
	// Keywords
%}

"await"                     { return KEY_AWAIT; }
"break"                     { return KEY_BREAK; }
"case"                      { return KEY_CASE; }
"catch"                     { return KEY_CATCH; }
"class"                     { return KEY_CLASS; }
"const"                     { return KEY_CONST; }
"continue"                  { return KEY_CONTINUE; }
"debugger"                  { return KEY_DEBUGGER; }
"default"                   { return KEY_DEFAULT; }
"delete"                    { return KEY_DELETE; }
"do"                        { return KEY_DO; }
"else"                      { return KEY_ELSE; }
"enum"                      { return KEY_ENUM; }
"export"                    { return KEY_EXPORT; }
"extends"                   { return KEY_EXTENDS; }
"false"                     { return KEY_FALSE; }
"finally"                   { return KEY_FINALLY; }
"for"                       { return KEY_FOR; }
"function"                  { return KEY_FUNCTION; }
"if"                        { return KEY_IF; }
"implement"                 { return KEY_IMPLEMENTS; }
"import"                    { return KEY_IMPORT; }
"in"                        { return KEY_IN; }
"instanceof"                { return KEY_INSTANCEOF; }
"interface"                 { return KEY_INTERFACE; }
"let"                       { return KEY_LET; }
"new"                       { return KEY_NEW; }
"null"                      { return KEY_NULL; }
"package"                   { return KEY_PACKAGE; }
"private"                   { return KEY_PRIVATE; }
"protected"                 { return KEY_PROTECTED; }
"public"                    { return KEY_PUBLIC; }
"return"                    { return KEY_RETURN; }
"super"                     { return KEY_SUPER; }
"switch"                    { return KEY_SWITCH; }
"static"                    { return KEY_STATIC; }
"this"                      { return KEY_THIS; }
"throw"                     { return KEY_THROW; }
"try"                       { return KEY_TRY; }
"true"                      { return KEY_TRUE; }
"typeof"                    { return KEY_TYPEOF; }
"var"                       { return KEY_VAR; }
"void"                      { return KEY_VOID; }
"while"                     { return KEY_WHILE; }
"with"                      { return KEY_WITH; }
"yield"                     { return KEY_YIELD; }

%{
	// Three character tokens
%}

"==="                       { return EQUAL_EQUAL_EQUAL; }
"!=="                       { return BANG_EQUAL_EQUAL; }
"**="                       { return STAR_STAR_EQUAL; }
"<<="                       { return LESS_LESS_EQUAL; }
">>="                       { return RIGHT_RIGHT_EQUAL; }
"&&="                       { return AND_AND_EQUAL; }
"||="                       { return PIPE_PIPE_EQUAL; }
">>>"                       { return RIGHT_RIGHT_RIGHT; }
"..."                       { return DOT_DOT_DOT; }

%{
	// Two character tokens
%}

"!="                        { return BANG_EQUAL; }
"=="                        { return EQUAL_EQUAL; }
">="                        { return GREATER_EQUAL; }
"<="                        { return LESS_EQUAL; }
"+="                        { return PLUS_EQUAL; }
"-="                        { return MINUS_EQUAL; }
"*="                        { return STAR_EQUAL; }
"/="                        { return SLASH_EQUAL; }
"&="                        { return AND_EQUAL; }
"|="                        { return PIPE_EQUAL; }
"^="                        { return CARET_EQUAL; }
"<<"                        { return LESS_LESS; }
">>"                        { return GREATER_GREATER; }
"&&"                        { return AND_AND; }
"||"                        { return PIPE_PIPE; }
"=>"                        { return ARROW; }
"??"                        { return QUESTION_QUESTION; }
"**"                        { return STAR_STAR; }
"++"                        { return PLUS_PLUS; }
"--"                        { return MINUS_MINUS; }
"?."                        { return QUESTION_DOT; }

%{
	// One character tokens
%}

"("                         { return LEFT_PAREN; }
")"                         { return RIGHT_PAREN; }
"{"                         { return LEFT_BRACE; }
"}"                         { return RIGHT_BRACE; }
"["                         { return LEFT_BRACKET; }
"]"                         { return RIGHT_BRACKET; }
","                         { return COMMA; }
"."                         { return DOT; }
"-"                         { return MINUS; }
"+"                         { return PLUS; }
"/"                         { return SLASH; }
"*"                         { return STAR; }
"%"                         { return MOD; }
";"                         { return SEMICOLON; }
"!"                         { return BANG; }
"="                         { return EQUAL; }
">"                         { return GREATER; }
"<"                         { return LESS; }
"&"                         { return AND; }
"|"                         { return PIPE; }
"~"                         { return TILDE; }
"^"                         { return CARET; }
"?"                         { return QUESTION; }

%{
	// Literals
%}

\"([^\\\"]|\\n|\\t)*\"      { return STRING; }
\'([^\\\']|\\n|\\t)*\'      { return STRING; }
-?([0-9]+|[0-9]*\.[0-9]+)   { return NUMBER; }

%{
	// Whitespace / comment
%}

"//"[^(\n|"\r\n")]*         { /* Comment */ }
"/*"[^"*/"]*"*/"            { /* Comment */ }
[ \t\n]                     { }

%{
	// Identifier
%}

[a-zA-Z_][a-zA-Z0-9_]*      { return IDENTIFIER; }

%{
	// Unknown Token
%}

.                           { std::cout << "Unknown token: " << yytext << "\n"; }

%%
