#![allow(warnings)]
use std::collections::hash_map::Entry;
use std::collections::{HashMap, HashSet};
use std::env::var;
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::ops::Index;
use std::slice::Iter;
use std::{array, default, fs, iter, mem};

use to_vec::ToVec;
use indexmap::IndexMap;

use crate::stage01_tokenizer::*;
use crate::stage02_ast_builder::{AstBuilder, ProgramRootNode};

//#[derive(Default)]
pub struct TypeChecker {
    pub ast_builder: AstBuilder,
}

impl TypeChecker {
    pub fn new(ast_builder: AstBuilder) -> TypeChecker {
        TypeChecker {
            ast_builder
        }
    }

    pub fn build_new_ast_with_types() -> ProgramRootNode {
        ProgramRootNode { children: vec![] }
    }
}
