%{
/* Lab2 Attention: You are only allowed to add code in this file and start at Line 26.*/
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"
#include <ctype.h>

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}

void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

/*
* Please don't modify the lines above.
* You can add C declarations of your own below.
*/
int commentLevel = 0;


/* @function: getstr
 * @input: a string literal
 * @output: the string value for the input which has all the escape sequences 
 * translated into their meaning.
 */

/* method design:
	1. comments are always braced inside a pair of '/'* and *'/', but layer by layer, use a global 
	integer commentLevel to record current layer level of comment
	2. strings always begin and end with '"', all char legal inside, but escape character is needed
	for special characters, control characters are always transfered
	3. error handling always made through the function EM_error
	4. end-of-file not specified, caused not needed, which already recognized through the end of string
*/


char *getstr(const char *str)
{
	//optional: implement this function if you need it
	int len = strlen(str);
	char *res = (char*) checked_malloc(len+1);
	int cur = 1;
	int dig_num = 0;
	while(str[cur] != '"'){
		if(str[cur] == '\\'){
			// handle the escape character
			cur++;
			if(isdigit(str[cur])){
				if(cur+4>len){
					res[dig_num++] = str[cur];
				}
				else{
					// try to transfer a ascii code like /ddd into char
					char d1 = str[cur];
					char d2 = str[++cur];
					char d3 = str[++cur];
					if(isdigit(d1) && isdigit(d2) && isdigit(d3)){
						int num1 = d1 - '0';
						int num2 = d2 - '0';
						int num3 = d3 - '0';
						int ascii =  100 * num1 + 10 * num2 + num3;
						res[dig_num++] = ascii;
					}	
				}
			}
			else{
				switch(str[cur]){
					// handle other escape characters
					case '\\': res[dig_num++] = '\\'; break;
					case 'n': res[dig_num++] = '\n'; break;
					case 't': res[dig_num++] = '\t'; break;
					case '"': res[dig_num++] = '"'; break;
					case '^':{
						if(cur != len-2){
							cur++;
							char u = str[cur];
							if(u >= 'A' && u <= 'Z'){
								res[dig_num++] = u - 'A' + 1;
							}
						}
						break;
					}
					case ' ': case '\n': case '\t':{
						// inside a string a newline or a tab is permitted 
						// but followed char in the next line should be a "\"
						while(str[cur] == ' ' | str[cur] == '\n' | str[cur] == '\t'){
							cur++;
						}
						if(str[cur] != '\\'){
							printf("begin with : %c !!!",str[cur]);
							EM_error(charPos, "newline not begin with \\ in a string");
							return NULL;
						}
						break;
					}
					default: break;
				}
			}
			cur++;
		}
		else{
			res[dig_num++] = str[cur++];
		}
	}
	if(dig_num == 0){
		free(res);
		return NULL;
	}
	res[dig_num] = '\0';
	return res;
}

%}
  /* You can add lex definitions here. */
%Start COMMENT
%%
  /* 
  * Below is an example, which you can wipe out
  * and write reguler expressions and actions of your own.
  */
<INITIAL>"/*" {adjust(); commentLevel++; BEGIN COMMENT;}
<COMMENT>"/*" {adjust(); commentLevel++;}
<COMMENT>"*/" {adjust(); commentLevel--;
	if(commentLevel == 0){
		BEGIN INITIAL;
	}
}
<COMMENT>.|\n {adjust();}


<INITIAL>"," {adjust(); return COMMA;}
<INITIAL>":" {adjust(); return COLON;}
<INITIAL>";" {adjust(); return SEMICOLON;}
<INITIAL>"(" {adjust(); return LPAREN;}
<INITIAL>")" {adjust(); return RPAREN;}
<INITIAL>"[" {adjust(); return LBRACK;}
<INITIAL>"]" {adjust(); return RBRACK;}
<INITIAL>"{" {adjust(); return LBRACE;}
<INITIAL>"}" {adjust(); return RBRACE;}
<INITIAL>"." {adjust(); return DOT;}
<INITIAL>"+" {adjust(); return PLUS;}
<INITIAL>"-" {adjust(); return MINUS;}
<INITIAL>"*" {adjust(); return TIMES;}
<INITIAL>"/" {adjust(); return DIVIDE;}
<INITIAL>"=" {adjust(); return EQ;}
<INITIAL>"<>" {adjust(); return NEQ;}
<INITIAL>"<" {adjust(); return LT;}
<INITIAL>"<=" {adjust(); return LE;}
<INITIAL>">" {adjust(); return GT;}
<INITIAL>">=" {adjust(); return GE;}
<INITIAL>"&" {adjust(); return AND;}
<INITIAL>"|" {adjust(); return OR;}
<INITIAL>let {adjust();return LET;}
<INITIAL>":=" {adjust(); return ASSIGN;}
<INITIAL>array {adjust(); return ARRAY;}
<INITIAL>if {adjust(); return IF;}
<INITIAL>then {adjust(); return THEN;}
<INITIAL>else {adjust(); return ELSE;}
<INITIAL>while {adjust(); return WHILE;}
<INITIAL>for {adjust(); return FOR;}
<INITIAL>to {adjust(); return TO;}
<INITIAL>do {adjust(); return DO;}
<INITIAL>in {adjust(); return IN;}
<INITIAL>end {adjust(); return END;}
<INITIAL>of {adjust(); return OF;}
<INITIAL>break {adjust(); return BREAK;}
<INITIAL>nil {adjust(); return NIL;}
<INITIAL>function {adjust(); return FUNCTION;}
<INITIAL>var {adjust(); return VAR;}
<INITIAL>type {adjust();return TYPE;}
<INITIAL>[a-zA-Z][a-zA-Z0-9_]* {adjust(); yylval.sval = String(yytext); return ID;}
<INITIAL>\"([^\"]*(\\\")*)*\" {adjust(); yylval.sval = getstr(yytext); return STRING;}
<INITIAL>[0-9]+ {adjust(); yylval.ival = atoi(yytext); return INT;}
"\n" {adjust(); EM_newline(); continue;}
<INITIAL>[\ \t]+ {adjust();}
