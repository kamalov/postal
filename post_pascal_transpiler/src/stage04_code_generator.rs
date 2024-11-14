use std::borrow::Borrow;
use std::collections::{HashMap, HashSet};
use std::collections::hash_map::Entry;
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::{array, default, fs};

use indexmap::IndexMap;
use to_vec::ToVec;

use crate::stage01_tokenizer::*;
use crate::stage02_ast_builder::*;

const OPERATORS_MAP: [(&str, &str); 3] = [
    ("+", " + "),
    ("-", " - "),
    ("=", " == "),
];

struct CurrentFunctionContext {
    function_node: FnData,
    iterators: Vec<(String, String)>,
    iterators_count: u32,
}

pub struct CodeGenerator<'ast> {
    ast_builder: &'ast AstBuilder,
    operators_map: HashMap<String, String>,
    current_function_context: Option<CurrentFunctionContext>,
}

const PADDING: &str = "    ";

impl<'ast> CodeGenerator<'ast> {
    pub fn new(ast_bulder: &AstBuilder) -> CodeGenerator {
        let mut operators_map = HashMap::new();
        for (op_from, op_to) in OPERATORS_MAP {
            operators_map.insert(op_from.to_string(), op_to.to_string());
        }

        CodeGenerator { 
            current_function_context: None,
            operators_map,
            ast_builder: ast_bulder
        }
    }

    pub fn generate_code(&mut self) -> String {
        let mut r = String::new();
        for top_node_id in &self.ast_builder.root_ids {
            let node = self.ast_builder.ast_arena.get(*top_node_id).unwrap();
            match node {
                AstNode::Fn(function_node) => {
                    let code = self.generate_function_code(function_node, "");
                    r.push_str(code.as_str());
                }
                _ => (),
            }
        }

        r
    }

    fn generate_function_code(&mut self, function_node: &FnData, padding: &str) -> String {
        self.current_function_context = Some(CurrentFunctionContext { function_node: function_node.clone(), iterators: vec![], iterators_count: 0 });

        let fn_params_code = self.generate_function_declaration_params_code(&function_node.params);
        let fn_body_code = self.generate_block_code(&function_node.body, padding);

        let mut r = String::new();
        let return_type_str = "".to_string(); //type_info_to_str(&function_node.return_type);

        writeln!(&mut r, "{}{} {}({}) {{", padding, return_type_str, function_node.name, fn_params_code);

        for d in &function_node.vars {
            let s = "".to_string(); // type_info_to_str(&d.1);
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
            let t = "".to_string(); // type_info_to_str(type_info);
            let s = format!("{} {}", t, name);
            parts.push(s);
        }

        parts.join(", ")
    }

    fn generate_block_code(&mut self, b: &BlockNode, padding: &str) -> String {
        let mut r = String::new();

        let mut s = vec![];
        for statement_id in &b.statement_id_list {
            let padding = format!("{}{}", padding, PADDING);
            let statement = self.ast_builder.ast_arena.get(*statement_id).unwrap();
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
            AstNode::VariableAssignment{variable_name, rexpression_id: right_side_id} => {
                let s = self.generate_variable_assignment_code(variable_name, *right_side_id, padding);
                r.push_str(s.as_str());
            }
            _ => {
                println!("{:?}", ast_node);
                panic!();
            }
        }

        r
    }

    fn generate_variable_assignment_code(&mut self, var_name: &String, node_id: AstNodeId, padding: &str) -> String {
        let mut r = String::new();

        let node = self.ast_builder.ast_arena.get(node_id).unwrap();
        let s = self.generate_expression_code(node);
        writeln!(&mut r, "{}{} = {};", padding, var_name, s);

        r
    }

    fn generate_expression_code(&mut self, node: &AstNode) -> String {
        let mut r = String::new();

        match &node {
            AstNode::Identifier{name, ..} => {
                write!(&mut r, "{}", name);
            }
            AstNode::Operator(op_value) => {
                write!(&mut r, " {} ", op_value);
            }
            AstNode::Integer(int_literal) => {
                write!(&mut r, "{}", int_literal);
            }
            AstNode::BinaryOperation{ type_info, operation, left_id, right_id } => {
                let left_node = self.ast_builder.ast_arena.get(*left_id).unwrap();
                let right_node = self.ast_builder.ast_arena.get(*right_id).unwrap();
                let left = self.generate_expression_code(left_node);
                let right = self.generate_expression_code(right_node);
                let mut op = operation.clone();
                op = self.operators_map.get(&op).cloned().unwrap_or(op);
                write!(&mut r, "{left}{op}{right}");
            }
            panic_ => {
                println!("{:?}", panic_);
                panic!();
            }
        }

        r
    }
}
