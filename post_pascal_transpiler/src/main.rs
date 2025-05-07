#![allow(warnings)]

mod stage01_tokenizer;
mod stage02_ast_builder;
mod stage03_type_checker;
mod stage04_code_generator;
mod type_info;
mod utils;

use std::collections::{HashMap, HashSet};
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::path::Path;
use std::{array, default, fs};

use to_vec::ToVec;

use stage01_tokenizer::*;
use stage02_ast_builder::*;
use stage03_type_checker::*;
use stage04_code_generator::*;

fn handle_error(token: Token, expected: &String, text: &String, tokenizer: &Tokenizer, prelude_lines_count: usize) {
    let lines = text.lines().to_vec();
    let (line_number, column_number) = tokenizer.char_index_to_line_and_column(token.char_index);
    let line = lines[line_number].clone();
    let (left, right) = if column_number >= line.len() {
        (line, "")
    } else {
        line.split_at(column_number)
    };
    println!("error: line {}, column {}\n", line_number + 1 - prelude_lines_count, column_number + 1);
    print!("{}", left);
    println!("\x1b[93m{}\x1b[0m\n", right);
    let v = token.value;
    let v = if v == "\n" { "line end".to_string() } else { v };
    println!("\x1b[93m{}\x1b[0m, token: \x1b[93m{}\x1b[0m", expected, v);
}

fn main() {
    let filename = Path::new("./tests/test_shared_pointer.post");
    //let filename = Path::new("./../aoc2024/aoc2024.post");
    let text = read_to_string(filename).unwrap();
    let prelude_lines_count = read_to_string(Path::new("./prelude.post")).unwrap().lines().to_vec().len() + 1;
    let prelude = include_str!("./../prelude.post");
    let text = format!("{}\n{}", prelude, text);
    //let prelude_lines_count = 0;
    
    let tokenizer = Tokenizer::create_and_parse(&text);

    let mut ast_builder = AstBuilder::new(&tokenizer);
    let ast_builder_result = ast_builder.parse_tokens_to_ast();
    if ast_builder_result.is_err() {
        let ast_error = ast_builder_result.unwrap_err();
        handle_error(ast_error.token, &ast_error.expected, &text, &tokenizer, prelude_lines_count);
    }

    let mut type_checker = TypeChecker::new(ast_builder.clone());
    let ast_with_types_result = type_checker.build_new_ast_with_types();
    let ast = match ast_with_types_result {
        Ok(ast) => ast,
        Err(err) => {
            handle_error(err.token, &err.expected, &text, &tokenizer, prelude_lines_count);
            return;
        }
    };

    let mut generator = CodeGenerator::new(ast);
    let generated_code = generator.generate_code();
    println!("\x1b[93m{generated_code}\x1b[0m");
    let filename = Path::new("./../template.cpp");
    let text = read_to_string(filename).unwrap();
    let text = text.replace("%GENERATED_CODE%", &generated_code);
    fs::write("./../generated.cpp", text);
}
