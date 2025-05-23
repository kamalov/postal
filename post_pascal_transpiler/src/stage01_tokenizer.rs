use std::collections::{HashMap, HashSet};
use std::fs::{read_dir, read_to_string};
use std::hash::Hash;
use std::{array, default, fs};
use to_vec::ToVec;

#[derive(Debug, Clone, PartialEq)]
pub enum TokenKind {
    Eof,
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

#[derive(Debug, Clone, PartialEq)]
pub struct Token {
    pub kind: TokenKind,
    pub value: String,
    pub char_index: usize,
}

impl Token {
    pub fn is_eof(&self) -> bool {
        self.kind == TokenKind::Eof
    }
}

const KEYWORDS: [&str; 29] = [
    "begin", "end", "do", //
    "function", "external", "var", "if", "for", "in", "loop", "break", "continue", "return", //
    "integer", "real", "string", "record", //
    "div", "mod", 
    "yes", "no", "not", "and", "or", 
    "xor", "shr", "shl", "bit_and", "bit_or"//
];

const SPECIALS: [&str; 18] = [
    "#", //
    "{", "}", "(", ")", "[", "]", ",", //
    ":", ";", "=", "*", "/", "+", "-", "<", ">", ".", //, "::", "<<", "=>", "->"
];

fn is_identifier_char(c: char) -> bool {
    !c.is_whitespace() && (c as u32) < 128 && (c.is_alphanumeric() || c == '_' || c == '$')
}

fn is_identifier_starter_char(c: char) -> bool {
    !c.is_whitespace() && (c as u32) < 128 && c.is_alphabetic() || c == '$'
}

#[derive(Default, Clone)]
pub struct Tokenizer {
    char_index: usize,
    text: String,
    keywords: HashSet<String>,
    specials: HashSet<String>,
    pub tokens: Vec<Token>,
    pub chars: Vec<char>,
}

impl Tokenizer {
    pub fn create_and_parse(text: &String) -> Tokenizer {
        let mut t = Tokenizer::default();

        for k in KEYWORDS {
            t.keywords.insert(k.to_string());
        }

        for c in SPECIALS {
            t.specials.insert(c.to_string());
        }
        
        t.chars = text.chars().to_vec();
        
        t.parse_chars_to_tokens();

        t
    }

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
        if self.char_index < self.chars.len() {
            let c = self.chars[self.char_index];
            self.char_index += 1;
            c
        } else {
            panic!();
        }
    }

    fn skip(&mut self) {
        if self.char_index < self.chars.len() {
            self.char_index += 1;
        } else {
            panic!();
        }
    }

    fn peek(&mut self) -> Option<char> {
        if self.char_index < self.chars.len() {
            Some(self.chars[self.char_index])
        } else {
            None
        }
    }

    fn peek_second(&mut self) -> Option<char> {
        if self.char_index < self.chars.len() - 1 {
            Some(self.chars[self.char_index + 1])
        } else {
            None
        }
    }

    fn check_next(&mut self, s: impl Into<String>) -> bool {
        let s: String = s.into();
        if self.char_index < self.chars.len() - s.len() + 1 {
            let chars = &self.chars[self.char_index..(self.char_index + s.len())];
            let v: String = chars.iter().collect();
            v == s
        } else {
            false
        }
    }

    fn parse_name(&mut self) -> Token {
        let mut value = String::new();
        let index = self.char_index;

        while let Some(c) = self.peek() {
            if is_identifier_char(c) {
                value.push(self.get_next_char())
            } else {
                break;
            }
        }

        let kind = if self.keywords.contains(&value) {
            if ["div", "mod", "and", "or", "not", "bit_and", "bit_or", "shr", "shl", "xor"].contains(&value.as_str()) {
                TokenKind::SpecialSymbol
            } else if ["yes", "no"].contains(&value.as_str()) {
                TokenKind::BooleanLiteral
            } else {
                TokenKind::Keyword 
            }
        } else {
            TokenKind::Identifier
        };

        Token { kind, value, char_index: index }
    }

    fn parse_number(&mut self) -> Token {
        let mut value = String::new();
        let index = self.char_index;

        let mut has_dot = false;
        loop {
            if self.check_next("..") {
                break;
            }

            if let Some(c) = self.peek() {
                if c.is_numeric() {
                    value.push(self.get_next_char());
                } else if c == '.' {
                    if has_dot {
                        break;
                    }
                    has_dot = true;
                    value.push(self.get_next_char());
                } else if c == '_' {
                    self.skip()
                } else {
                    break;
                }
            }
        }

        let mut kind = if has_dot { TokenKind::RealLiteral } else { TokenKind::IntegerLiteral };
        Token { kind, value, char_index: index }
    }

    fn parse_string(&mut self) -> Token {
        let mut value = String::new();
        let index = self.char_index;

        self.skip();
        loop {
            if self.check_next("''") {
                self.skip();
                self.skip();
                value.push('\'');
                continue;
            }

            if let Some(c) = self.peek() {
                if c == '\'' {
                    self.skip();
                    break;
                }
                value.push(self.get_next_char());
                continue;
            }

            break;
        }

        Token {
            kind: TokenKind::String,
            value,
            char_index: index,
        }
    }

    fn parse_comment(&mut self) -> Token {
        let mut value = String::new();
        let index = self.char_index;

        self.skip();
        self.skip();
        while let Some(c) = self.peek() {
            if c == '\n' {
                break;
            }
            if c == '\r' {
                self.skip();
                continue;
            }
            value.push(self.get_next_char())
        }

        Token {
            kind: TokenKind::Comment,
            value,
            char_index: index,
        }
    }

    fn add_token(&mut self, s: impl Into<String>) {
        let t = Token {
            kind: TokenKind::SpecialSymbol,
            value: s.into(),
            char_index: self.char_index,
        };
        self.skip();
        self.skip();
        self.tokens.push(t);
    }

    fn parse_chars_to_tokens(&mut self) {
        self.tokens = vec![];

        loop {
            if self.peek().is_none() {
                break;
            };

            let c = self.peek().unwrap();

            if c == '\n' {
                let t = Token {
                    kind: TokenKind::LineEnd,
                    value: "\n".to_string(),
                    char_index: self.char_index,
                };
                self.skip();
                self.tokens.push(t);
                continue;
            }

            if c.is_whitespace() {
                self.skip();
                continue;
            }

            if is_identifier_starter_char(c) {
                let t = self.parse_name();
                self.tokens.push(t);
                continue;
            }

            if c.is_numeric() {
                let t = self.parse_number();
                self.tokens.push(t);
                continue;
            }

            if c == '\'' {
                let t = self.parse_string();
                self.tokens.push(t);
                continue;
            }

            if self.check_next("//") {
                let t = self.parse_comment();
                self.tokens.push(t);
                continue;
            }

            if self.check_next(">>") {
                self.add_token(">>");
                continue;
            }

            if self.check_next(">=") {
                self.add_token(">=");
                continue;
            }

            if self.check_next("<=") {
                self.add_token("<=");
                continue;
            }

            if self.check_next("<>") {
                self.add_token("<>");
                continue;
            }

            if self.check_next("..") {
                self.add_token("..");
                continue;
            }

            if self.specials.contains(&c.to_string()) {
                let t = Token {
                    kind: TokenKind::SpecialSymbol,
                    value: c.to_string(),
                    char_index: self.char_index,
                };
                self.skip();
                self.tokens.push(t);
                continue;
            }

            self.skip();
            println!("{:?}", c);
            todo!();
        }

        let eof = Token {
            kind: TokenKind::Eof,
            value: String::new(),
            char_index: self.chars.len() - 1,
        };
        self.tokens.push(eof);
    }

    pub fn debug_print_tokens(&self) {
        for t in &self.tokens {
            println!("\x1b[93m{:?}\x1b[0m {:?} ", t.value, t.kind);
        }
        println!("");
    }
}
