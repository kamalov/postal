#![allow(warnings)]

use std::collections::{HashMap, HashSet};
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::{array, default, fs};
use to_vec::ToVec;

use crate::stage01_tokenizer::*;
use crate::stage02_ast_builder::*;

pub struct Gen<'a> {
    root: &'a ProgramNode,
    current_function_node: Option<FunctionNode>,
    current_function_iterators: Vec<(String, String)>,
    current_function_it_var_name: String,
}

const PADDING: &str = "    ";

impl Gen<'_> {
    pub fn new<'a>(ast_bulder: &'a AstBuilder<'a>) -> Gen<'a> {
        Gen {
            root: &ast_bulder.root,
            current_function_node: None,
            current_function_iterators: vec![],
            current_function_it_var_name: "".to_string()
        }
    }

    pub fn generate_code(&mut self) -> String {
        let mut r = String::new();
        for n in &self.root.children {
            match n {
                AstNode::Function(f) => {
                    let code = self.generate_function_code(f, "");
                    r.push_str(code.as_str());
                }
                _ => (),
            }
        }

        r
    }

    fn generate_function_code(&mut self, f: &FunctionNode, padding: &str) -> String {
        fn type_name_to_type_name_str(ti: &String) -> String {
            match ti.as_str() {
                "int" => "integer".to_string(),
                "float" => "single".to_string(),
                "bool" => "boolean".to_string(),
                _ => ti.clone()
            }
        }

        fn type_info_to_str(ti: &TypeInfo) -> String {
            let type_name = type_name_to_type_name_str(&ti.type_str);

            if ti.is_array {
                "array of ".to_owned() + &type_name
            } else {
                type_name
            }
        }

        self.current_function_node = Some(f.clone());
        let s = self.generate_block_code(&f.body, padding);

        let mut r = String::new();

        writeln!(&mut r, "{}procedure {};", padding, f.name);

        if f.vars.len() > 0 {
            writeln!(&mut r, "var");
            for d in &f.vars {
                let s = type_info_to_str(&d.1);
                writeln!(&mut r, "    {}: {};", d.0.clone(), s);
            }
        }

        if self.current_function_iterators.len() > 0 {
            for (it_name, it_type) in &self.current_function_iterators {
                writeln!(&mut r, "    {}: {};", it_name, type_name_to_type_name_str(&it_type));
                writeln!(&mut r, "    {}_index: {};", it_name, &"integer".to_string());
            }
        }

        writeln!(&mut r, "begin");
        write!(&mut r, "{s}");
        writeln!(&mut r, "end;");
        writeln!(&mut r, "");

        r
    }

    fn generate_block_code(&mut self, b: &BlockNode, padding: &str) -> String {
        let mut r = String::new();

        let mut s = vec![];
        for statement in &b.statements {
            let padding = format!("{}{}", padding, PADDING);
            let statement_code = self.generate_statement_code(statement, padding.as_str());
            s.push(statement_code);
            // match statement {
            //     AstNode::FunctionCall(_) | AstNode::VariableDeclaration(_) => {
            //         s.push(format!("{statement_code};"));
            //     }
            //     else_ => {
            //         s.push(format!("{statement_code}"));
            //     },
            // }
        }
        let s = s.join("");
        write!(&mut r, "{s}");

        r
    }

    fn generate_statement_code(&mut self, statement: &AstNode, padding: &str) -> String {
        let mut r = String::new();

        match statement {
            AstNode::IfStatement(st) => {
                let s = self.generate_if_statement(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::ForStatement(st) => {
                let s = self.generate_for_statement(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::LoopStatement(st) => {
                let s = self.generate_loop_statement(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::Iteration(st) => {
                let s = self.generate_iteration(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::BreakStatement() => {
                writeln!(&mut r, "{}break;", padding);
            }
            AstNode::FunctionCall(st) => {
                let s = self.generate_function_call(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::VariableAssignment(v) => {
                let s = self.generate_variable_assignment(v, padding);
                r.push_str(s.as_str());
            }
            AstNode::Comment(c) => {
                writeln!(&mut r, "{}//{}", padding, c.value);
            }
            _ => (),
        }

        r
    }

    fn generate_if_statement(&mut self, f: &IfStatementNode, padding: &str) -> String {
        let mut r = String::new();

        for i in 0..f.if_blocks.len() {
            let (condition, block) = &f.if_blocks[i];
            let expr = self.generate_expression_code(&condition.root);
            let block = self.generate_block_code(&block, padding);

            writeln!(&mut r, "");
            if i == 0 {
                writeln!(&mut r, "{}if {} then begin", padding, expr);
            } else {
                writeln!(&mut r, "{}else if {} then begin", padding, expr);
            }
            write!(&mut r, "{}", block);
            write!(&mut r, "{}end", padding);
        }

        if let Some(block) = &f.else_block {
            let block_str = self.generate_block_code(&block, padding);
            writeln!(&mut r, "");
            writeln!(&mut r, "{}else begin", padding);
            write!(&mut r, "{}", block_str);
            write!(&mut r, "{}end", padding);
        }

        writeln!(&mut r, ";");

        r
    }

    fn generate_for_statement(&mut self, f: &ForStatementNode, padding: &str) -> String {
        let mut r = String::new();

        todo!();
        let index_variable_name = format!("_{}_index", f.iterator_variable_name);
        // decls.push((index_variable_name.clone(), "integer".to_owned()));

        let iteratable = self.generate_expression_code(&f.iteratable.root);
        let block = self.generate_block_code(&f.for_block, padding);
        writeln!(&mut r, "");
        writeln!(
            &mut r,
            "{}for {} := Low({}) to High({}) do begin",
            padding, index_variable_name, iteratable, iteratable
        );
        writeln!(
            &mut r,
            "{}{}{} := ({})[{}];",
            padding, PADDING, f.iterator_variable_name, iteratable, index_variable_name
        );

        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}end;", padding);

        r
    }

    fn generate_loop_statement(&mut self, f: &LoopStatementNode, padding: &str) -> String {
        let mut r = String::new();

        let block = self.generate_block_code(&f.block, padding);
        writeln!(&mut r, "");
        writeln!(&mut r, "{}while true do begin", padding);
        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}end;", padding);

        r
    }

    fn generate_iteration(&mut self, it: &IterationNode, padding: &str) -> String {
        let mut r = String::new();

        let f = self.current_function_node.as_ref().unwrap();
        let index = self.current_function_iterators.len();
        let iteratable = f.vars.get(&it.var_name).unwrap();
        let it_var_name = format!("{}__it{}", it.var_name, index);
        let it_index_var_name = format!("{}_index", it_var_name);
        let it_var_type = iteratable.type_str.clone();
        self.current_function_iterators.push((it_var_name.clone(), it_var_type.clone()));
        self.current_function_it_var_name = it_var_name.clone();
        // decls.push((index_variable_name.clone(), "integer".to_owned()));

        let block = self.generate_block_code(&it.block, padding);
        writeln!(&mut r, "");
        writeln!(
            &mut r,
            "{}for {} := 0 to Length({}) - 1 do begin",
            padding, it_index_var_name, it.var_name,
        );
        writeln!(
            &mut r,
            "{}{}{} := {}[{}];",
            padding, PADDING, it_var_name, it.var_name, it_index_var_name
        );

        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}end;", padding);

        r
    }

    fn generate_function_call(&mut self, f: &FunctionCallNode, padding: &str) -> String {
        let mut r = String::new();

        let mut params = vec![];
        for p in &f.params {
            match &*p.expression.root {
                AstNode::Identifier(n) => {
                    if n.value == "it" {
                        let name = self.current_function_it_var_name.as_str();
                        params.push(name.to_string());    
                    } else if n.value == "it_index" {
                        let name = self.current_function_it_var_name.as_str();
                        params.push(name.to_string() + "_index");    
                    } else {
                        params.push(n.value.to_string());
                    }
                }
                _ => {
                    let s = self.generate_expression_code(&*p.expression.root);
                    params.push(s);
                }
            }
        }
        let p = params.join(", ");
        let name = f.function_name.as_str();
        match name {
            "log" => {
                writeln!(&mut r, "{}writeln({});", padding, p);
            }

            "push" => {
                let name = &params[0];
                let value = &params[1];
                writeln!(
                    &mut r,
                    "{}SetLength({}, length({}) + 1);",
                    padding, name, name
                );
                writeln!(
                    &mut r,
                    "{}{}[length({}) - 1] := {};",
                    padding, name, name, value
                );
            }

            _ => {
                writeln!(&mut r, "{}{}({});", padding, name, p);
            }
        }

        r
    }

    fn generate_variable_assignment(&mut self, v: &VariableAssignmentNode, padding: &str) -> String {
        let mut r = String::new();

        let s = self.generate_expression_code(&*v.rvalue_expression.root);
        writeln!(&mut r, "{}{} := {};", padding, v.name, s);

        r
    }

    fn generate_expression_code(&mut self, node: &AstNode) -> String {
        let mut r = String::new();

        match &node {
            AstNode::Identifier(f) => {
                write!(&mut r, "{}", f.value);
            }
            AstNode::Operator(o) => {
                write!(&mut r, " {} ", o.value);
            }
            AstNode::IntegerLiteral(f) => {
                write!(&mut r, "{}", f.value);
            }
            AstNode::FloatLiteral(f) => {
                write!(&mut r, "{}", f.value);
            }
            AstNode::StringLiteral(f) => {
                write!(&mut r, "'{}'", f.value);
            }
            AstNode::BinaryOperation(b) => {
                let left = self.generate_expression_code(&b.children[0]);
                let right = self.generate_expression_code(&b.children[1]);
                //write!(&mut r, "({} {} {})", left, b.operation.value, right);
                write!(&mut r, "{} {} {}", left, b.operation.value, right);
            }
            a => {
                println!("{:?}", a);
                panic!();
            }
        }

        r
    }
}
