use std::collections::{HashMap, HashSet};
use std::fs::{read_dir, read_to_string};
use std::hash::Hash;
use std::{array, default, fs};

use crate::tokenizer::*;

pub const KEYWORDS: [&str; 29] = [
    "begin", "end", "do", //
    "function", "external", "var", "if", "for", "in", "loop", "break", "continue", "return", //
    "integer", "real", "string", "record", //
    "div", "mod", //
    "yes", "no", "not", "and", "or", //
    "xor", "shr", "shl", "bit_and", "bit_or"//
];

pub const SPECIAL_SYMBOLS: [&str; 18] = [
    "#", //
    "{", "}", "(", ")", "[", "]", ",", //
    ":", ";", "=", "*", "/", "+", "-", "<", ">", ".", //
    // , "::", "<<", "=>", "->"
];

pub struct Compiler {
    pub keywords: HashSet<&'static str>,
    pub special_symbols: HashSet<&'static str>,
    pub source_text: String,
    pub tokens: Vec<Token>
}

impl Token {
    pub fn get_string_value(&self, c: &Compiler) -> String {
        c.get_token_value(self).to_string()
    }
}

impl Compiler {
    pub fn new(source_text: String) -> Self {
        Self {
            keywords: HashSet::from_iter(KEYWORDS.iter().cloned()),
            special_symbols: HashSet::from_iter(SPECIAL_SYMBOLS.iter().cloned()),
            source_text,
            tokens: vec![]
        }
    }

    pub fn get_token_value(&self, token: &Token) -> &str {
        let fci = token.first_char_index as usize;
        let length = token.length as usize;
        let s = &self.source_text[fci..fci + length];
        return s;
    }

    pub fn get_token_value_by_token_id(&self, token_id: TokenId) -> &str {
        let token = &self.tokens[token_id as usize];
        self.get_token_value(token)
    }
}
