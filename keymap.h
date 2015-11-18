/*
 * keymap.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef KEYMAP_H
#define KEYMAP_H

#if defined KBD_US

static unsigned char key_map[] = {
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',  127,    9,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  '[',  ']',  '\n',   0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'',  '`',    0, '\\',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '<',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
	   0 };

static unsigned char shift_map[] = {
       0,   27,  '!',  '@',  '#',  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+',  127,    9,
	 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
	 'O',  'P',  '{',  '}',   13,    0,  'A',  'S',
	 'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
     '"',  '~',  '0',  '|',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '<',  '>',  '?',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '>',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };

static unsigned char alt_map[] = {
       0,    0,    0,  '@',    0,  '$',    0,    0,
     '{',   '[',  ']', '}', '\\',    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,  '~',   13,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,  '|',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };

static unsigned char num_map[] = "789-456+1230,";
static unsigned int  pad_map[] = { 7, 8, 9, 0, 4, 5, 6, 0, 1, 2, 3, 0, 0 };

static char *func_map[] = {
    "\033[[A", "\033[[B", "\033[[C", "\033[[D",
    "\033[[E", "\033[[F", "\033[[G", "\033[[H",
    "\033[[I", "\033[[J", "\033[[K", "\033[[L"
};

#elif defined KBD_IT

static unsigned char key_map[] = {
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', '\'',    0,  127,    9,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',    0,  '+', '\n',    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',    0,
       0, '\\',    0,    0,  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '-',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '<',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };

static unsigned char shift_map[] = {
       0,   27,  '!',  '"',    0,  '$',  '%',  '&',
     '/',  '(',  ')',  '=',  '?',  '^',  127,    9,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',    0,  '*',   13,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',    0,
       0,  '|',  '0',    0,  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ';',  ':',  '_',    0,    0,
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,  '>',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };

static unsigned char alt_map[] = {
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '[',  ']',   13,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '@',
     '#',    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };

static unsigned char num_map[] = "789-456+1230,";
static unsigned int  pad_map[] = { 7, 8, 9, 0, 4, 5, 6, 0, 1, 2, 3, 0, 0 };

static char *func_map[] = {
    "\033[[A", "\033[[B", "\033[[C", "\033[[D",
    "\033[[E", "\033[[F", "\033[[G", "\033[[H",
    "\033[[I", "\033[[J", "\033[[K", "\033[[L"
};

#else
#error "Keyboard-type not supported"
#endif

#endif
