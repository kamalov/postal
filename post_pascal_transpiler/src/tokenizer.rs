use std::collections::{HashMap, HashSet};
use std::fs::{read_dir, read_to_string};
use std::hash::Hash;
use std::{array, default, fs};

use crate::compiler::Compiler;

fn is_identifier_char(c: char) -> bool {
    !c.is_whitespace() && (c as u32) < 128 && (c.is_alphanumeric() || c == '_' || c == '$')
}

fn is_identifier_starter_char(c: char) -> bool {
    !c.is_whitespace() && (c as u32) < 128 && c.is_alphabetic() || c == '$'
}

#[derive(Debug, Copy, Clone, PartialEq)]
pub enum TokenKind {
    LineEnd,
    SpecialSymbol,
    Identifier,
    Keyword,
    String,
    IntegerLiteral,
    RealLiteral,
    BooleanLiteral,
    Comment,
}

pub type TokenId = u32;

#[derive(Debug, Copy, Clone, PartialEq)]
pub struct Token {
    pub kind: TokenKind,
    pub first_char_index: usize,
    pub length: usize,
}

struct Tokenizer<'compiler> {
    compiler: &'compiler Compiler,
    source_text: &'compiler str,
    chars: Vec<char>,
    cursor: usize,
    tokens: Vec<Token>,
}

pub fn fill_tokens<'src>(compiler: &Compiler) -> Vec<Token> {
    let mut tokenizer = Tokenizer {
        compiler,
        source_text: &compiler.source_text,
        chars: compiler.source_text.chars().collect(),
        cursor: 0,
        tokens: vec![],
    };
    tokenizer.parse_chars_to_tokens();
    return tokenizer.tokens;
}

pub fn debug_print_tokens(compiler: &Compiler) {
    for token in &compiler.tokens {
        let s = &compiler.source_text[token.first_char_index..token.first_char_index + token.length];
        println!("\x1b[93m{:?}\x1b[0m {:?} ", s, token.kind);
    }
    println!("");
}

impl<'compiler> Tokenizer<'compiler> {
    pub fn char_index_to_line_and_column(&self, index: usize) -> (usize, usize) {
        let mut line = 0;
        let mut last_new_line_index = 0;
        let mut prev_char = 0 as char;
        for i in 0..(index + 1) {
            if prev_char == '\n' {
                line += 1;
                last_new_line_index = i;
            }
            prev_char = self.chars[i];
        }

        let column = index - last_new_line_index;
        (line, column)
    }

    fn get_next_char(&mut self) -> char {
        if self.cursor < self.chars.len() {
            let c = self.chars[self.cursor];
            self.cursor += 1;
            c
        } else {
            panic!();
        }
    }

    fn skip_char(&mut self) {
        if self.cursor < self.chars.len() {
            self.cursor += 1;
        } else {
            panic!();
        }
    }

    fn peek_char(&mut self) -> Option<char> {
        if self.cursor < self.chars.len() {
            Some(self.chars[self.cursor])
        } else {
            None
        }
    }

    fn peek_second_char(&mut self) -> Option<char> {
        if self.cursor < self.chars.len() - 1 {
            Some(self.chars[self.cursor + 1])
        } else {
            None
        }
    }

    fn check_next(&mut self, s: impl Into<String>) -> bool {
        let s: String = s.into();
        if self.cursor < self.chars.len() - s.len() + 1 {
            let chars = &self.chars[self.cursor..(self.cursor + s.len())];
            let v: String = chars.iter().collect();
            v == s
        } else {
            false
        }
    }

    fn parse_name(&mut self) {
        let first_char_index = self.cursor;

        while let Some(c) = self.peek_char() {
            if is_identifier_char(c) {
                self.get_next_char();
            } else {
                break;
            }
        }

        let value = &self.source_text[first_char_index..self.cursor];

        let kind = if self.compiler.keywords.contains(value) {
            if ["div", "mod", "and", "or", "not", "bit_and", "bit_or", "shr", "shl", "xor"].contains(&value) {
                TokenKind::SpecialSymbol
            } else if ["yes", "no"].contains(&value) {
                TokenKind::BooleanLiteral
            } else {
                TokenKind::Keyword
            }
        } else {
            TokenKind::Identifier
        };

        self.tokens.push(Token {
            kind,
            first_char_index,
            length: value.len(),
        });
    }

    fn parse_number(&mut self) {
        let first_char_index = self.cursor;

        let mut has_dot = false;
        loop {
            if self.check_next("..") {
                break;
            }

            if let Some(c) = self.peek_char() {
                if c.is_numeric() {
                    self.get_next_char();
                } else if c == '.' {
                    if has_dot {
                        break;
                    }
                    has_dot = true;
                    self.get_next_char();
                } else if c == '_' {
                    self.skip_char()
                } else {
                    break;
                }
            }
        }

        let value = &self.source_text[first_char_index..self.cursor];
        self.tokens.push(Token {
            kind: if has_dot { TokenKind::RealLiteral } else { TokenKind::IntegerLiteral },
            first_char_index,
            length: value.len(),
        });
    }

    fn parse_string(&mut self) {
        let first_char_index = self.cursor;

        self.skip_char();
        loop {
            if self.check_next("''") {
                self.skip_char();
                self.skip_char();
                continue;
            }

            if let Some(c) = self.peek_char() {
                if c == '\'' {
                    self.skip_char();
                    break;
                }
                self.get_next_char();
                continue;
            }

            break;
        }

        let value = &self.source_text[first_char_index..self.cursor];
        self.tokens.push(Token {
            kind: TokenKind::String,
            first_char_index,
            length: value.len(),
        });
    }

    fn parse_comment(&mut self) {
        let first_char_index = self.cursor;

        self.skip_char();
        self.skip_char();
        while let Some(c) = self.peek_char() {
            if c == '\n' {
                break;
            }
            if c == '\r' {
                self.skip_char();
                continue;
            }
            self.get_next_char();
        }

        let value = &self.source_text[first_char_index..self.cursor];
        self.tokens.push(Token {
            kind: TokenKind::Comment,
            first_char_index,
            length: value.len(),
        });
    }

    fn add_token(&mut self, s: &str, kind: TokenKind) {
        let first_char_index = self.cursor;
        for _ in 0..s.len() {
            self.skip_char();
        }
        let value = &self.source_text[first_char_index..self.cursor];
        self.tokens.push(Token {
            kind,
            first_char_index,
            length: value.len(),
        });
    }

    fn parse_chars_to_tokens(&mut self) {
        loop {
            if self.peek_char().is_none() {
                break;
            };

            let c = self.peek_char().unwrap();

            if c == '\n' {
                self.add_token("\n", TokenKind::LineEnd);
                continue;
            }

            if c.is_whitespace() {
                self.skip_char();
                continue;
            }

            if is_identifier_starter_char(c) {
                let t = self.parse_name();
                continue;
            }

            if c.is_numeric() {
                let t = self.parse_number();
                continue;
            }

            if c == '\'' {
                let t = self.parse_string();
                continue;
            }

            if self.check_next("//") {
                let t = self.parse_comment();
                continue;
            }

            if self.check_next(">>") {
                self.add_token(">>", TokenKind::SpecialSymbol);
                continue;
            }

            if self.check_next(">=") {
                self.add_token(">=", TokenKind::SpecialSymbol);
                continue;
            }

            if self.check_next("<=") {
                self.add_token("<=", TokenKind::SpecialSymbol);
                continue;
            }

            if self.check_next("<>") {
                self.add_token("<>", TokenKind::SpecialSymbol);
                continue;
            }

            if self.check_next("..") {
                self.add_token("..", TokenKind::SpecialSymbol);
                continue;
            }

            if self.compiler.special_symbols.contains(&c.to_string().as_str()) {
                self.add_token(&c.to_string(), TokenKind::SpecialSymbol);
                continue;
            }

            println!("unexpected token {:?}", c);
            panic!();
        }
    }
}
