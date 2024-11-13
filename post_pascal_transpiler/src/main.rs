#![allow(warnings)]

use std::collections::{HashMap, HashSet};
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::{array, default, fs};
use stage02_ast_builder::AstBuilderError;
use to_vec::ToVec;

mod stage01_tokenizer;
mod stage02_ast_builder;
mod stage03_type_checker;
mod stage04_code_generator;

pub fn readln() {
    let mut guess = String::new();
    std::io::stdin().read_line(&mut guess).unwrap();
}

fn hadle_ast_builder_error(err: AstBuilderError, text: &String, tokenizer: &stage01_tokenizer::Tokenizer) {
    let lines = text.lines().to_vec();
    let (line_number, column_number) = tokenizer.char_index_to_line_and_column(err.token.char_index);
    let line = lines[line_number].clone();
    let (left, right) = line.split_at(column_number);
    println!(
        "parse error, line {}, column {}",
        line_number + 1, column_number + 1
    );
    println!("");
    print!("{}", left);
    println!("\x1b[93m{}\x1b[0m", right);
    println!("");
    let v = err.token.value;
    let v = if v == "\n" {"line end".to_string()} else { v };
    println!(
        "\x1b[93m{}\x1b[0m, token: \x1b[93m{}\x1b[0m",
        err.expected, v
    );
}

fn main() {
    let filename = "D:/src/postal/post_pascal_transpiler/main.post";
    let text = read_to_string(filename.to_string()).unwrap();
    let mut tokenizer = stage01_tokenizer::Tokenizer::create_and_parse(&text);
    //tokenizer.print_tokens();
    println!("tokenized");

    let mut ast_builder = stage02_ast_builder::AstBuilder::new(&tokenizer);
    let ast_builder_result = ast_builder.parse_tokens_to_ast();
    println!("ast built\n");

    if ast_builder_result.is_err() {
        hadle_ast_builder_error(ast_builder_result.unwrap_err(), &text, &tokenizer);
    }

    let mut type_checker = stage03_type_checker::TypeChecker::new(ast_builder.clone());
    
    let mut generator = stage04_code_generator::CodeGenerator::new(&ast_builder);
    let generated_code = generator.generate_code();
    println!("\x1b[93m{generated_code}\x1b[0m");

    let filename = "D:\\src\\postal\\template.cpp";
    let text = read_to_string(filename.to_string()).unwrap();
    let text = text.replace("%GENERATED_CODE%", &generated_code);
    fs::write("D:\\src\\postal\\generated.cpp", text);
}
