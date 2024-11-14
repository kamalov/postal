use std::collections::hash_map::Entry;
use std::collections::{HashMap, HashSet};
use std::env::var;
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::slice::Iter;
use std::{array, default, fs, iter, mem};
use std::rc::Rc;

use to_vec::ToVec;
use indexmap::IndexMap;

use crate::stage01_tokenizer::*;
use crate::stage02_ast_builder::*;

//#[derive(Default)]
pub struct TypeChecker {
    pub ast_builder: AstBuilder,
}

impl TypeChecker {
    pub fn new(ast_builder: &AstBuilder) -> TypeChecker {
        TypeChecker {
            ast_builder: ast_builder.clone()
        }
    }

    pub fn fill_types_in_ast(&mut self) {
        for id in &self.ast_builder.root_ids {
            let node = self.ast_builder.need_node(id);
            match node {
                AstNode::Fn(_) => {
                    //self.process_fn(id);
                }
                _ => panic!()
            }
        }
    }

    pub fn get_expression_type_info(&self, id: &AstNodeId) -> TypeInfo {
        TypeInfo::default()
    }

    pub fn process_fn(&mut self, id: &AstNodeId) {
        let fn_node = self.ast_builder.need_node(id);
        if let AstNode::Fn(data) = fn_node {
            for statement_id in &data.body.statement_id_list {
                let statement_node = self.ast_builder.need_node(statement_id);
                match statement_node {
                    AstNode::VariableAssignment { variable_name, rexpression_id } => {
                        let type_info = self.get_expression_type_info(rexpression_id);
                        //data.return_type = type_info;
                    }
                    _ => panic!()
                }
            }
        } else {
            panic!();
        }
    }
}
