#![allow(warnings)]

use std::collections::{HashMap, HashSet};
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::{array, default, fs};
use to_vec::ToVec;

mod stage01_tokenizer;
mod stage02_ast_builder;
mod stage03_code_generator;

pub fn readln() {
    let mut guess = String::new();
    std::io::stdin().read_line(&mut guess).unwrap();
}

fn main() {
    let filename = "D:/src/postal/post_pascal_transpiler/main.post";
    let text = read_to_string(filename.to_string()).unwrap();
    let mut tokenizer = stage01_tokenizer::Tokenizer::create_and_parse(&text);
    //tokenizer.print_tokens();
    println!("tokenized");

    let mut ast_builder = stage02_ast_builder::AstBuilder::new(&tokenizer);
    let r = ast_builder.parse_tokens_to_ast();
    println!("ast built\n");
    
    if let Err(e) = r {
        let lines = text.lines().to_vec();
        let (line_number, column_number) = tokenizer.char_index_to_line_column(e.token.char_index);
        let line = lines[line_number - 1];
        let (left, right) = line.split_at(column_number - 1);
        println!(
            "parse error, line {}, column {}",
            line_number, column_number
        );
        println!("");
        print!("{}", left);
        println!("\x1b[93m{}\x1b[0m", right);
        println!("");
        let v = e.token.value;
        let v = if v == "\n" {"line end".to_string()} else { v };
        println!(
            "expected: \x1b[93m{}\x1b[0m, found: \x1b[93m{}\x1b[0m",
            e.expected, v
        );
        return;
    }

    let mut generator = stage03_code_generator::Gen::new(&ast_builder);
    let generated_code = generator.generate_code();
    println!("\x1b[93m{generated_code}\x1b[0m");

    let filename = "D:\\src\\postal\\template.cpp";
    let text = read_to_string(filename.to_string()).unwrap();
    let text = text.replace("%GENERATED_CODE%", &generated_code);
    fs::write("D:\\src\\postal\\main.cpp", text);
}
