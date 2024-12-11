#![allow(warnings)]

mod stage01_tokenizer;
mod stage02_ast_builder;
mod stage03_type_checker;
mod stage04_code_generator;

use std::collections::{HashMap, HashSet};
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::{array, default, fs};
use std::path::Path;

use to_vec::ToVec;

use stage01_tokenizer::*;
use stage02_ast_builder::*;
use stage03_type_checker::*;
use stage04_code_generator::*;

pub fn readln() {
    let mut guess = String::new();
    std::io::stdin().read_line(&mut guess).unwrap();
}

fn hadle_error(token: Token, expected: &String, text: &String, tokenizer: &Tokenizer) {
    let lines = text.lines().to_vec();
    let (line_number, column_number) = tokenizer.char_index_to_line_and_column(token.char_index);
    let line = lines[line_number].clone();
    let (left, right) = line.split_at(column_number);
    println!(
        "error: line {}, column {}",
        line_number + 1, column_number + 1
    );
    println!("");
    print!("{}", left);
    println!("\x1b[93m{}\x1b[0m", right);
    println!("");
    let v = token.value;
    let v = if v == "\n" {"line end".to_string()} else { v };
    println!(
        "\x1b[93m{}\x1b[0m, token: \x1b[93m{}\x1b[0m",
        expected, v
    );
}

fn main() {
    //let filename = Path::new("./test.post");
    let filename = Path::new("./../aoc2024/aoc2024.post");
    let text = read_to_string(filename).unwrap();
    let mut tokenizer = stage01_tokenizer::Tokenizer::create_and_parse(&text);
    //tokenizer.print_tokens();
    println!("tokenized");

    let mut ast_builder = AstBuilder::new(&tokenizer);
    let ast_builder_result = ast_builder.parse_tokens_to_ast();
    println!("ast built\n");

    if ast_builder_result.is_err() {
        let ast_error = ast_builder_result.unwrap_err();
        hadle_error(ast_error.token, &ast_error.expected, &text, &tokenizer);
    }

    let mut type_checker = TypeChecker::new(ast_builder.clone());
    let ast_with_types = type_checker.build_new_ast_with_types();

    let generated_code;
    match ast_with_types {
        Ok(ast_with_types) => {
            let mut generator = CodeGenerator::new(ast_with_types);
            generated_code = generator.generate_code();
            println!("\x1b[93m{generated_code}\x1b[0m");
        }
        Err(err) => {
            hadle_error(err.token, &err.expected, &text, &tokenizer);
            return;
        }
    }

    let filename = Path::new("./../template.cpp");
    let text = read_to_string(filename).unwrap();
    let text = text.replace("%GENERATED_CODE%", &generated_code);
    fs::write("./../generated.cpp", text);
}