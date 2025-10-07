use std::collections::{HashMap, HashSet};
use std::fs::{read_dir, read_to_string};
use std::hash::Hash;
use std::{array, default, fs};

use crate::compiler::Compiler;

pub type TokenId = u32;

pub struct TokenError {
    pub char_index: usize,
    pub message: String,
}

impl TokenError {
    pub fn new(char_index: usize, s: impl Into<String>) -> Self {
        Self {
            char_index,
            message: s.into(),
        }
    }
}

#[derive(Debug, Copy, Clone, PartialEq)]
pub enum TokenKind {
    Eof,
    LineEnd,
    SpecialSymbol,
    Identifier,
    Keyword,
    StringLiteral,
    IntegerLiteral,
    RealLiteral,
    BooleanLiteral,
    Comment,
}

#[derive(Debug, Copy, Clone, PartialEq)]
pub struct Token {
    pub id: TokenId,
    pub kind: TokenKind,
    pub first_char_index: u32,
    pub length: u16,
}

pub fn parse_tokens(compiler: &Compiler) -> Result<Vec<Token>, TokenError> {
    let mut tokenizer = Tokenizer {
        compiler,
        source_text: &compiler.source_text,
        chars: compiler.source_text.chars().collect(),
        current_token_index: 0,
        tokens: vec![],
    };
    tokenizer.parse_chars_to_tokens()?;
    
    return Ok(tokenizer.tokens);
}

struct Tokenizer<'compiler> {
    compiler: &'compiler Compiler,
    source_text: &'compiler str,
    chars: Vec<char>,
    current_token_index: usize,
    tokens: Vec<Token>,
}

impl Tokenizer<'_> {
    fn get_next_char(&mut self) -> char {
        if self.current_token_index < self.chars.len() {
            let c = self.chars[self.current_token_index];
            self.current_token_index += 1;
            c
        } else {
            panic!();
        }
    }

    fn skip_char(&mut self) {
        if self.current_token_index < self.chars.len() {
            self.current_token_index += 1;
        } else {
            panic!();
        }
    }

    fn peek_char(&mut self) -> Option<char> {
        if self.current_token_index < self.chars.len() {
            Some(self.chars[self.current_token_index])
        } else {
            None
        }
    }

    fn peek_second_char(&mut self) -> Option<char> {
        if self.current_token_index < self.chars.len() - 1 {
            Some(self.chars[self.current_token_index + 1])
        } else {
            None
        }
    }

    fn check_next(&mut self, s: impl Into<String>) -> bool {
        let s: String = s.into();
        if self.current_token_index < self.chars.len() - s.len() + 1 {
            let chars = &self.chars[self.current_token_index..(self.current_token_index + s.len())];
            let v: String = chars.iter().collect();
            v == s
        } else {
            false
        }
    }

    fn add_token(&mut self, kind: TokenKind, first_char_index: usize, length: usize) -> Token {
        let token = Token { id: self.tokens.len() as TokenId, kind, first_char_index: first_char_index.try_into().unwrap(), length: length.try_into().unwrap() };
        self.tokens.push(token);
        token
    }

    fn add_str_token(&mut self, s: &str, kind: TokenKind) {
        let first_char_index = self.current_token_index;
        for _ in 0..s.len() {
            self.skip_char();
        }
        let value = &self.source_text[first_char_index..self.current_token_index];
        self.add_token(kind, first_char_index, value.len());
    }

    fn parse_name(&mut self) {
        let first_char_index = self.current_token_index;

        while let Some(c) = self.peek_char() {
            if is_identifier_char(c) {
                self.get_next_char();
            } else {
                break;
            }
        }

        let value = &self.source_text[first_char_index..self.current_token_index];

        let kind = if self.compiler.keywords.contains(value) {
            if ["div", "mod", "and", "or", "not", "bit_and", "bit_or", "bit_shift_left", "bit_shift_right", "bit_xor"].contains(&value) {
                TokenKind::SpecialSymbol
            } else if ["true", "false"].contains(&value) {
                TokenKind::BooleanLiteral
            } else {
                TokenKind::Keyword
            }
        } else {
            TokenKind::Identifier
        };

        self.add_token(kind, first_char_index, value.len());
    }

    fn parse_number(&mut self) {
        let first_char_index = self.current_token_index;

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

        let value = &self.source_text[first_char_index..self.current_token_index];
        let kind = if has_dot { TokenKind::RealLiteral } else { TokenKind::IntegerLiteral }; 
        self.add_token(kind, first_char_index, value.len());
    }

    fn parse_string(&mut self) {
        let first_char_index = self.current_token_index;

        self.skip_char();
        loop {
            if self.check_next("\\\"") {
                self.skip_char();
                self.skip_char();
                continue;
            }

            if let Some(c) = self.peek_char() {
                if c == '"' {
                    self.skip_char();
                    break;
                }
                self.get_next_char();
                continue;
            }

            break;
        }

        let value = &self.source_text[first_char_index..self.current_token_index];
        self.add_token(TokenKind::StringLiteral, first_char_index, value.len());
    }

    fn parse_comment(&mut self) {
        let first_char_index = self.current_token_index;

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

        let value = &self.source_text[first_char_index..self.current_token_index];
        self.add_token(TokenKind::Comment, first_char_index, value.len());
    }

    fn parse_chars_to_tokens(&mut self) -> Result<(), TokenError> {
        loop {
            if self.peek_char().is_none() {
                break;
            };

            let c = self.peek_char().unwrap();

            if c == '\n' {
                self.add_str_token("\n", TokenKind::LineEnd);
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

            if c == '"' {
                let t = self.parse_string();
                continue;
            }

            if self.check_next("//") {
                let t = self.parse_comment();
                continue;
            }

            if self.check_next(">>") {
                self.add_str_token(">>", TokenKind::SpecialSymbol);
                continue;
            }

            if self.check_next(">=") {
                self.add_str_token(">=", TokenKind::SpecialSymbol);
                continue;
            }

            if self.check_next("<=") {
                self.add_str_token("<=", TokenKind::SpecialSymbol);
                continue;
            }

            if self.check_next("<>") {
                self.add_str_token("<>", TokenKind::SpecialSymbol);
                continue;
            }

            if self.check_next("..") {
                self.add_str_token("..", TokenKind::SpecialSymbol);
                continue;
            }

            if self.compiler.special_symbols.contains(&c.to_string().as_str()) {
                self.add_str_token(&c.to_string(), TokenKind::SpecialSymbol);
                continue;
            }

            return Err(TokenError::new(self.current_token_index, "unexpected char"));
        }

        self.add_token(TokenKind::Eof, self.source_text.len(), 0);

        return Ok(());
    }
}

/// utils 

pub fn debug_print_tokens(compiler: &Compiler) {
    for token in &compiler.tokens {
        let s = &compiler.get_token_value(token);
        println!("\x1b[93m{:?}\x1b[0m {:?} ", s, token.kind);
    }
    println!("");
}

fn is_identifier_char(c: char) -> bool {
    !c.is_whitespace() && (c as u32) < 128 && (c.is_alphanumeric() || c == '_' || c == '$')
}

fn is_identifier_starter_char(c: char) -> bool {
    !c.is_whitespace() && (c as u32) < 128 && c.is_alphabetic() || c == '$'
}
