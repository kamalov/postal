#![allow(warnings)]

use std::collections::{HashMap, HashSet};
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::{array, default, fs};
use to_vec::ToVec;

use crate::stage01_tokenizer::*;
use crate::stage02_ast_builder::*;

struct CurrentFunctionContext {
    function_node: FunctionNode,
    iterators: Vec<(String, String)>,
    iterators_count: u32,
}

pub struct Gen<'a> {
    root: &'a ProgramNode,
    current_function_context: Option<CurrentFunctionContext>,
}

const PADDING: &str = "    ";

fn convert_type_name_str(ti: &String) -> String {
    match ti.as_str() {
        "i64" => "long".to_string(),
        "f64" => "double".to_string(),
        //"bool" => "boolean".to_string(),
        _ => panic!(), // ti.clone()
    }
}

impl Gen<'_> {
    pub fn new<'a>(ast_bulder: &'a AstBuilder<'a>) -> Gen<'a> {
        Gen {
            root: &ast_bulder.root,
            current_function_context: None,
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

    fn generate_function_code(&mut self, function_node: &FunctionNode, padding: &str) -> String {
        fn type_info_to_str(ti: &TypeInfo) -> String {
            let type_name = convert_type_name_str(&ti.type_str);

            if ti.is_array {
                format!("std::vector<{}>", type_name)
            } else {
                type_name
            }
        }

        self.current_function_context = Some(CurrentFunctionContext {
            function_node: function_node.clone(),
            iterators: vec![],
            iterators_count: 0,
        });

        let s = self.generate_block_code(&function_node.body, padding);

        let mut r = String::new();

        writeln!(&mut r, "{}void {}() {{", padding, function_node.name);

        for d in &function_node.vars {
            let s = type_info_to_str(&d.1);
            writeln!(&mut r, "    {} {};", s, d.0.clone());
        }

        let iterators = &self.current_function_context.as_ref().unwrap().iterators;
        for (it_name, it_type) in iterators {
            //writeln!(&mut r, "    {}: {};", it_name, convert_type_name_str(&it_type));
            //writeln!(&mut r, "    {}_index: {};", it_name, &"int".to_string());
        }

        write!(&mut r, "{s}");
        writeln!(&mut r, "}}");
        writeln!(&mut r, "");

        self.current_function_context = None;

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
                writeln!(&mut r, "{}if ({}) {{", padding, expr);
            } else {
                writeln!(&mut r, "{}else if ({}) {{", padding, expr);
            }
            write!(&mut r, "{}", block);
            write!(&mut r, "{}}}", padding);
        }

        if let Some(block) = &f.else_block {
            let block_str = self.generate_block_code(&block, padding);
            writeln!(&mut r, "");
            writeln!(&mut r, "{}else {{", padding);
            write!(&mut r, "{}", block_str);
            write!(&mut r, "{}}}", padding);
        }

        writeln!(&mut r, ";");

        r
    }

    fn generate_for_statement(&mut self, f: &ForStatementNode, padding: &str) -> String {
        let mut r = String::new();

        todo!();
        let index_variable_name = format!("_{}_index", f.iterator_variable_name);
        // decls.push((index_variable_name.clone(), "i64".to_owned()));

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
        writeln!(&mut r, "{}for (;;) {{", padding);
        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}}}", padding);

        r
    }

    fn generate_iteration(&mut self, iteration_node: &IterationNode, padding: &str) -> String {
        let mut r = String::new();

        let mut function_context = self.current_function_context.as_mut().unwrap();
        let index = function_context.iterators_count;
        function_context.iterators_count += 1;
        let iteratable = function_context
            .function_node
            .vars
            .get(&iteration_node.iteratable_name)
            .unwrap();
        let iteratable_name = iteration_node.iteratable_name.clone();
        let it_name = format!("{}__it{}", iteratable_name, index);
        let it_index_name = format!("{}_index", it_name);
        let it_type = iteratable.type_str.clone();
        function_context
            .iterators
            .push((it_name.clone(), it_type.clone()));
        // decls.push((index_variable_name.clone(), "i64".to_owned()));

        let block = self.generate_block_code(&iteration_node.block, padding);
        writeln!(&mut r, "");
        writeln!(
            &mut r,
            "{0}for (int {1} = 0; {1} < {2}.size(); {1}++) {{",
            padding, it_index_name, iteratable_name
        );
        writeln!(
            &mut r,
            "{}{}{} {} = {}[{}];",
            padding, PADDING, convert_type_name_str(&it_type), it_name, iteratable_name, it_index_name
        );

        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}}}", padding);

        self.current_function_context
            .as_mut()
            .unwrap()
            .iterators
            .pop();

        r
    }

    fn generate_function_call(&mut self, f: &FunctionCallNode, padding: &str) -> String {
        let mut r = String::new();

        let mut param_names = vec![];
        for function_call_param in &f.function_call_params {
            let s = self.generate_expression_code(&*function_call_param.expression.root);
            param_names.push(s);
        }
        let p = param_names.join(", ");
        let function_name = f.function_name.as_str();
        match function_name {
            "log" => {
                if f.function_call_params.is_empty() {
                    writeln!(&mut r,"{}printf(\"\\n\");", padding);
                } else {
                    let mut format_parts = vec![];
                    let mut names = vec![];
                    for (i, param) in f.function_call_params.iter().enumerate() {
                        let param_name = param_names[i].clone();
                        match param.expression.type_str.as_str() {
                            "i64" => {
                                format_parts.push("%lld");
                                names.push(format!("static_cast<long long>({})", param_name));
                            }
                            "f64" => {
                                format_parts.push("%f");
                                names.push(param_name.clone());
                            }
                            "string" => {
                                format_parts.push("%s");
                                names.push(param_name.clone());
                            }
                            _ => {
                                println!("{:?}", p);
                                panic!();
                            }
                        }
                    }
                    writeln!(
                        &mut r,
                        "{}printf(\"{}\\n\", {});",
                        padding,
                        format_parts.join(" "),
                        names.join(", ")
                    );
                }
            }

            "push" => {
                let name = &param_names[0];
                let value = &param_names[1];
                writeln!(&mut r, "{}{}.push_back({});", padding, name, value);
            }

            _ => {
                writeln!(&mut r, "{}{}({});", padding, function_name, p);
            }
        }

        r
    }

    fn generate_variable_assignment(
        &mut self,
        v: &VariableAssignmentNode,
        padding: &str,
    ) -> String {
        let mut r = String::new();

        let s = self.generate_expression_code(&*v.rvalue_expression.root);
        writeln!(&mut r, "{}{} = {};", padding, v.name, s);

        r
    }

    fn generate_expression_code(&mut self, node: &AstNode) -> String {
        let mut r = String::new();

        match &node {
            AstNode::Identifier(identifier_node) => {
                if identifier_node.value == "it" || identifier_node.value == "it_index" {
                    match &self.current_function_context {
                        None => {
                            panic!("it or it_index is reserved and allowed only inside iterators inside functions");
                        }
                        Some(current_function_context) => {
                            let iterators = &current_function_context.iterators;
                            if iterators.is_empty() {
                                panic!(
                                    "it or it_index is reserved and allowed only inside iterators"
                                );
                            }

                            let (it_name, it_type_str) = iterators.last().unwrap();
                            match identifier_node.value.as_str() {
                                "it" => {
                                    write!(&mut r, "{}", it_name.clone());
                                }
                                "it_index" => {
                                    write!(&mut r, "{}", it_name.clone() + "_index");
                                }
                                _ => panic!(),
                            }
                        }
                    }
                } else {
                    write!(&mut r, "{}", identifier_node.value);
                }
            }
            AstNode::Operator(o) => {
                write!(&mut r, " {} ", o.value);
            }
            AstNode::IntegerLiteral(f) => {
                write!(&mut r, "{}", f.value);
            }
            AstNode::RealLiteral(f) => {
                write!(&mut r, "{}", f.value);
            }
            AstNode::StringLiteral(f) => {
                write!(&mut r, "\"{}\"", f.value);
            }
            AstNode::BinaryOperation(b) => {
                let left = self.generate_expression_code(&b.children[0]);
                let right = self.generate_expression_code(&b.children[1]);
                //write!(&mut r, "({} {} {})", left, b.operation.value, right);
                if b.operation.value == "=" {
                    write!(&mut r, "{} {} {}", left, "==", right);
                } else {
                    write!(&mut r, "{} {} {}", left, b.operation.value, right);
                }
            }
            a => {
                println!("{:?}", a);
                panic!();
            }
        }

        r
    }
}
