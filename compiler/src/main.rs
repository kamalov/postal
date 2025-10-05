#![allow(warnings)]

mod compiler;
mod tokenizer;
mod ast_builder;
mod type_checker;
mod code_generator;
mod type_info;
mod utils;

use std::collections::{HashMap, HashSet};
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::path::Path;
use std::{array, default, fs};

use indexmap::{IndexMap, IndexSet};
use std::mem;

use compiler::*;
use tokenizer::*;
use ast_builder::*;
use type_checker::*;
use code_generator::*;

fn main() {
    //let filename = Path::new("./tests/test_all.post");
    let filename = Path::new("./../examples/aoc2024/aoc2024.post");
    let source_text = read_to_string(filename).unwrap();
    let prelude_text = read_to_string(Path::new("./prelude.post")).unwrap();
    let mut prelude_lines_count = prelude_text.lines().collect::<Vec<_>>().len();
    if prelude_text.chars().last().unwrap() == '\n' {
        prelude_lines_count += 1;
    }
    let prelude = include_str!("./../prelude.post");
    let source_text = format!("{prelude}\n{source_text}");
    
    let mut compiler = Compiler::new(source_text);
    
    // tokenizing
    let tokenizer_result = parse_tokens(&compiler);
    match tokenizer_result {
        Ok(tokens) => compiler.tokens = tokens,
        Err(token_error) => {
            handle_tokenizer_error(&compiler, token_error.char_index, &token_error.message, prelude_lines_count);
            return;
        }
    }
    //debug_print_tokens(&compiler);

    // building ast
    let ast_builder_result = build_ast(&compiler);
    match ast_builder_result {
        Ok(ast) => compiler.ast = ast,
        Err(ast_error) => {
            handle_error(&compiler, ast_error.token, &ast_error.message, prelude_lines_count);
            return;
        }
    }

    // filling types
    let ast_with_types_result = build_new_ast_with_types(&compiler);
    match ast_with_types_result {
        Ok(ast) => compiler.ast = ast,
        Err(type_check_error) => {
            handle_error(&compiler, type_check_error.token, &type_check_error.message, prelude_lines_count);
            return;
        }
    }

    // generating C++ code
    let generated_code_text = generate_code(&compiler);
    println!("\x1b[93m{generated_code_text}\x1b[0m");
    let text = format!("#include \"prelude.cpp\"\n\n{}", generated_code_text);
    fs::write("./cpp/generated.cpp", text);
}

///
 
fn char_index_to_line_and_column(chars: &Vec<char>, index: usize) -> (usize, usize) {
    let mut line = 0;
    let mut last_new_line_index = 0;
    let mut prev_char = 0 as char;
    for i in 0..(index + 1) {
        if prev_char == '\n' {
            line += 1;
            last_new_line_index = i;
        }
        prev_char = chars[i];
    }

    let column = index - last_new_line_index;
    (line, column)
}

fn handle_tokenizer_error(compiler: &Compiler, char_index: usize, message: &String, prelude_lines_count: usize) {
    let lines = compiler.source_text.lines().collect::<Vec<_>>();
    let chars = compiler.source_text.chars().collect::<Vec<_>>();
    let (line_number, column_number) = char_index_to_line_and_column(&chars, char_index);
    let line = lines[line_number].clone();
    let (left, right) = if column_number >= line.len() {
        (line, "")
    } else {
        line.split_at(column_number)
    };
    println!("error: line {}, column {}\n", line_number + 1 - prelude_lines_count, column_number + 1);
    print!("{}", left);
    println!("\x1b[93m{}\x1b[0m\n", right);
    let v = compiler.source_text.chars().nth(char_index).unwrap();
    let v = if v == '\n' { "line end".to_string() } else { v.to_string() };
    println!("\x1b[93m{}\x1b[0m, token: \x1b[93m{}\x1b[0m", message, v);
}

fn handle_error(compiler: &Compiler, token: Token, message: &String, prelude_lines_count: usize) {
    let lines = compiler.source_text.lines().collect::<Vec<_>>();
    let chars = compiler.source_text.chars().collect::<Vec<_>>();
    let (line_number, column_number) = char_index_to_line_and_column(&chars, token.first_char_index as usize);
    let line = lines[line_number].clone();
    let (left, right) = if column_number >= line.len() {
        (line, "")
    } else {
        line.split_at(column_number)
    };
    println!("error: line {}, column {}\n", line_number + 1 - prelude_lines_count, column_number + 1);
    print!("{}", left);
    println!("\x1b[93m{}\x1b[0m\n", right);
    let v = compiler.get_token_value(&token);
    let v = if v == "\n" { "line end" } else { v };
    println!("\x1b[93m{}\x1b[0m, token: \x1b[93m{}\x1b[0m", message, v);
}

