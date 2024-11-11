#![allow(warnings)]

use std::collections::{HashMap, HashSet};
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::{array, default, fs};

use indexmap::IndexMap;
use to_vec::ToVec;

use crate::stage01_tokenizer::*;
use crate::stage02_ast_builder::*;

struct CurrentFunctionContext {
    function_node: FunctionNode,
    iterators: Vec<(String, String)>,
    iterators_count: u32,
}

pub struct CodeGenerator {
    root: ProgramRootNode,
    current_function_context: Option<CurrentFunctionContext>,
}

const PADDING: &str = "    ";

fn convert_type_name_str(ti: &String) -> String {
    match ti.as_str() {
        "" => "void".to_string(),
        "int" => "long".to_string(),
        "real" => "double".to_string(),
        //"bool" => "boolean".to_string(),
        _ => panic!(), // ti.clone()
    }
}

fn type_info_to_str(ti: &TypeInfo) -> String {
    let type_name = convert_type_name_str(&ti.type_str);

    if ti.is_array {
        format!("std::vector<{}>", type_name)
    } else {
        type_name
    }
}

impl CodeGenerator {
    pub fn new(ast_bulder: &AstBuilder) -> CodeGenerator {
        CodeGenerator { root: ast_bulder.root.clone(), current_function_context: None }
    }

    pub fn generate_code(&mut self) -> String {
        let mut r = String::new();
        for n in &self.root.children.clone() {
            match n {
                AstNode::Record(record_node) => {
                    let code = self.generate_record_code(record_node, "");
                    r.push_str(code.as_str());
                }
                AstNode::Function(function_node) => {
                    let code = self.generate_function_code(function_node, "");
                    r.push_str(code.as_str());
                }
                _ => (),
            }
        }

        r
    }

    fn generate_record_code(&mut self, record_node: &RecordNode, padding: &str) -> String {
        let mut result_str = String::new();

        writeln!(&mut result_str, "{padding}struct {} {{", record_node.name);

        for (field_name, type_info) in &record_node.fields {
            let type_info_str = type_info_to_str(type_info);
            writeln!(&mut result_str, "{padding}{PADDING}{type_info_str} {field_name};");
        }

        writeln!(&mut result_str, "}};");
        writeln!(&mut result_str, "");

        result_str
    }

    fn generate_function_code(&mut self, function_node: &FunctionNode, padding: &str) -> String {
        self.current_function_context = Some(CurrentFunctionContext { function_node: function_node.clone(), iterators: vec![], iterators_count: 0 });

        let fn_params_code = self.generate_function_declaration_params_code(&function_node.params);
        let fn_body_code = self.generate_block_code(&function_node.body, padding);

        let mut r = String::new();
        let return_type_str = type_info_to_str(&function_node.return_type);

        writeln!(&mut r, "{}{} {}({}) {{", padding, return_type_str, function_node.name, fn_params_code);

        for d in &function_node.vars {
            let s = type_info_to_str(&d.1);
            writeln!(&mut r, "    {} {};", s, d.0.clone());
        }

        // let iterators = &self.current_function_context.as_ref().unwrap().iterators;
        // for (it_name, it_type) in iterators {
        //     writeln!(&mut r, "    {}: {};", it_name, convert_type_name_str(&it_type));
        //     writeln!(&mut r, "    {}_index: {};", it_name, &"int".to_string());
        // }

        write!(&mut r, "{fn_body_code}");
        writeln!(&mut r, "}}");
        writeln!(&mut r, "");

        self.current_function_context = None;

        r
    }

    fn generate_function_declaration_params_code(&self, params: &IndexMap<String, TypeInfo>) -> String {
        let mut parts = vec![];
        for (name, type_info) in params {
            let s = format!("{} {}", type_info_to_str(type_info), name);
            parts.push(s);
        }

        parts.join(", ")
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

    fn generate_statement_code(&mut self, ast_node: &AstNode, padding: &str) -> String {
        let mut r = String::new();

        match ast_node {
            AstNode::IfStatement(st) => {
                let s = self.generate_if_statement_code(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::ForStatement(st) => {
                let s = self.generate_for_statement_code(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::LoopStatement(st) => {
                let s = self.generate_loop_statement_code(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::Iteration(st) => {
                let s = self.generate_iteration_code(st, padding);
                r.push_str(s.as_str());
            }
            AstNode::FunctionCall(st) => {
                let s = self.generate_function_call_code(st);
                let s = format!("{}{};\n", padding, s);
                r.push_str(s.as_str());
            }
            AstNode::VariableAssignment(v) => {
                let s = self.generate_variable_assignment_code(v, padding);
                r.push_str(s.as_str());
            }
            AstNode::Group(v) => {
                let s = self.generate_group_code(v, padding);
                r.push_str(s.as_str());
            }
            AstNode::BreakStatement() => {
                writeln!(&mut r, "{}break;", padding);
            }
            AstNode::Comment(c) => {
                writeln!(&mut r, "{}//{}", padding, c.value);
            }
            AstNode::ReturnStatement(expression) => {
                writeln!(&mut r, "{}return {};", padding, self.generate_expression_code(&expression.root));
            }
            _ => (),
        }

        r
    }

    fn generate_if_statement_code(&mut self, f: &IfStatementNode, padding: &str) -> String {
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

    fn generate_for_statement_code(&mut self, f: &ForStatementNode, padding: &str) -> String {
        let mut r = String::new();

        todo!();
        let index_variable_name = format!("_{}_index", f.iterator_variable_name);
        // decls.push((index_variable_name.clone(), "int".to_owned()));

        let iteratable = self.generate_expression_code(&f.iteratable.root);
        let block = self.generate_block_code(&f.for_block, padding);
        writeln!(&mut r, "");
        writeln!(&mut r, "{}for {} := Low({}) to High({}) do begin", padding, index_variable_name, iteratable, iteratable);
        writeln!(&mut r, "{}{}{} := ({})[{}];", padding, PADDING, f.iterator_variable_name, iteratable, index_variable_name);

        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}end;", padding);

        r
    }

    fn generate_loop_statement_code(&mut self, f: &LoopStatementNode, padding: &str) -> String {
        let mut r = String::new();

        let block = self.generate_block_code(&f.block, padding);
        writeln!(&mut r, "");
        writeln!(&mut r, "{}for (;;) {{", padding);
        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}}}", padding);

        r
    }

    fn generate_iteration_code(&mut self, iteration_node: &IterationNode, padding: &str) -> String {
        let mut r = String::new();

        let mut function_context = self.current_function_context.as_mut().unwrap();
        let index = function_context.iterators_count;
        function_context.iterators_count += 1;
        let iteratable = function_context.function_node.vars.get(&iteration_node.iteratable_name).unwrap();
        let iteratable_name = iteration_node.iteratable_name.clone();
        let it_name = format!("{}__it{}", iteratable_name, index);
        let it_index_name = format!("{}_index", it_name);
        let it_type = iteratable.type_str.clone();
        function_context.iterators.push((it_name.clone(), it_type.clone()));
        // decls.push((index_variable_name.clone(), "int".to_owned()));

        let block = self.generate_block_code(&iteration_node.block, padding);
        writeln!(&mut r, "");
        writeln!(&mut r, "{0}for (int {1} = 0; {1} < {2}.size(); {1}++) {{", padding, it_index_name, iteratable_name);
        writeln!(&mut r, "{}{}{} {} = {}[{}];", padding, PADDING, convert_type_name_str(&it_type), it_name, iteratable_name, it_index_name);

        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}}}", padding);

        self.current_function_context.as_mut().unwrap().iterators.pop();

        r
    }

    fn generate_function_call_code(&mut self, f: &FunctionCallNode) -> String {
        let mut r = String::new();

        let mut param_names = vec![];
        for expression in &f.params_group.expressions {
            let s = self.generate_expression_code(&*expression.root);
            param_names.push(s);
        }
        let p = param_names.join(", ");
        let function_name = f.name.as_str();
        match function_name {
            "log" => {
                if f.params_group.expressions.is_empty() {
                    write!(&mut r, "printf(\"\\n\")");
                } else {
                    let mut format_parts = vec![];
                    let mut names = vec![];
                    for (i, expression) in f.params_group.expressions.iter().enumerate() {
                        let param_name = param_names[i].clone();
                        match expression.type_str.as_str() {
                            "int" => {
                                format_parts.push("%lld");
                                names.push(format!("static_cast<long long>({})", param_name));
                            }
                            "real" => {
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
                    write!(&mut r, "printf(\"{}\\n\", {})", format_parts.join(" "), names.join(", "));
                }
            }

            "push" => {
                let name = &param_names[0];
                let value = &param_names[1];
                write!(&mut r, "{}.push_back({})", name, value);
            }

            _ => {
                write!(&mut r, "{}({})", function_name, p);
            }
        }

        r
    }

    fn generate_group_code(&mut self, group: &GroupNode, padding: &str) -> String {
        let mut s = String::new();

        let mut expression_code_list = vec![];
        for expression in &group.expressions {
            let s = self.generate_expression_code(&*expression.root);
            expression_code_list.push(s);
        }
        let code = expression_code_list.join(", ");
        write!(&mut s, "{}({})", padding, code);

        s
    }

    fn generate_array_item_access_code(&mut self, array_item_access: &ArrayItemAccessNode) -> String {
        let mut s = String::new();

        let root = &*array_item_access.access_expression.root;
        let expression_code = self.generate_expression_code(root);

        write!(&mut s, "{}[{}]", array_item_access.array_name, expression_code);

        s
    }

    fn generate_variable_assignment_code(&mut self, v: &VariableAssignmentNode, padding: &str) -> String {
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
                                panic!("it or it_index is reserved and allowed only inside iterators");
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
            AstNode::Group(group) => {
                let code = self.generate_group_code(group, "");
                write!(&mut r, "{}", code);
            }
            AstNode::FunctionCall(function_call_node) => {
                let code = self.generate_function_call_code(function_call_node);
                write!(&mut r, "{}", code);
            }
            AstNode::ArrayItemAccess(array_item_access) => {
                let code = self.generate_array_item_access_code(array_item_access);
                write!(&mut r, "{}", code);
            }
            panic_ => {
                println!("{:?}", panic_);
                panic!();
            }
        }

        r
    }
}
