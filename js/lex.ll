%option noyywrap
%option yylineno

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
	// One character tokens
%}

"("                         { return LEFT_PAREN; }
")"                         { return RIGHT_PAREN; }
"{"                         { return LEFT_BRACE; }
"}"                         { return RIGHT_BRACE; }
","                         { return COMMA; }
"."                         { return DOT; }
"-"                         { return MINUS; }
"+"                         { return PLUS; }
"/"                         { return SLASH; }
"*"                         { return STAR; }
"%"                         { return MOD; }
";"                         { return SEMICOLON; }
"!"                         { return NOT; }
"="                         { return EQUAL; }
">"                         { return GREATER; }
"<"                         { return LESS; }
"&"                         { return AND; }
"|"                         { return PIPE; }

%{
	// Two+ character tokens
%}

"!="                        { return NOT_EQUAL; }
"=="                        { return EQUAL_EQUAL; }
"==="                       { return EQUAL_EQUAL_EQUAL; }
"!=="                       { return NOT_EQUAL_EQUAL; }
">="                        { return GREATER_EQUAL; }
"<="                        { return LESS_EQUAL; }
"<<"                        { return LESS_LESS; }
">>"                        { return GREATER_GREATER; }
"&&"                        { return AND_AND; }
"||"                        { return PIPE_PIPE; }

%{
	// Literals
%}

\"([^\\\"]|\\n|\\t)*\"      { return STRING; }
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
