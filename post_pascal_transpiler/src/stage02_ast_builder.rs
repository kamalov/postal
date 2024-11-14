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
use id_arena::{Arena, Id};

use crate::stage01_tokenizer::*;

pub type AstNodeId = Id<AstNode>;

#[derive(Debug, Clone)]
pub struct FunctionDeclarationParam {
    pub name: String,
    pub type_info: TypeInfo,
}

#[derive(Debug, Clone)]
pub struct FnData {
    pub name: String,
    pub params: IndexMap<String, TypeInfo>,
    pub vars: HashMap<String, TypeInfo>,
    pub body: BlockNode,
    pub return_type: TypeInfo,
}

#[derive(Debug, Clone)]
pub struct RecordNode {
    pub name: String,
    pub fields: IndexMap<String, TypeInfo>,
}

#[derive(Debug, Clone)]
pub struct BlockNode {
    pub statement_id_list: Vec<AstNodeId>,
}

#[derive(Debug, Clone)]
pub struct FunctionParamNode {
    pub expression: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct VariableAssignmentNode {
    pub name: String,
    pub rvalue_expression: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct AssignmentNode {
    pub lvalue: ExpressionNode,
    pub rvalue: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct VariableDeclarationNode {
    pub name: String,
    //pub var_type: String,
}

#[derive(Debug, Clone)]
pub struct FunctionCallNode {
    pub name: String,
    pub params_group: GroupNode,
}

#[derive(Debug, Clone)]
pub struct GroupNode {
    pub token: Token,
    pub expressions: Vec<ExpressionNode>,
}

#[derive(Debug, Clone)]
pub struct ArrayBracketsNode {
    pub expression: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct ArrayItemAccessNode {
    pub array_name: String,
    pub access_expression: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct IntegerLiteralNode {
    pub value: String,
}

#[derive(Debug, Clone)]
pub struct RealLiteralNode {
    pub value: String,
}

#[derive(Debug, Clone)]
pub struct StringLiteralNode {
    pub value: String,
}

#[derive(Debug, Clone)]
pub struct IdentifierNode {
    pub token: Token,
    pub value: String,
}

#[derive(Debug, Clone)]
pub struct OperatorNode {
    pub value: String,
}

#[derive(Debug, Clone)]
pub struct CommentNode {
    pub value: String,
}

#[derive(Clone, Copy)]
pub struct Test {
    pub type_str: i32,
    //pub children: Vec<AstNode>,
}

#[derive(Debug, Clone)]
pub struct IfStatementNode {
    pub if_blocks: Vec<(ExpressionNode, BlockNode)>,
    pub else_block: Option<BlockNode>,
}

#[derive(Debug, Clone)]
pub struct LoopStatementNode {
    pub block: BlockNode,
}

#[derive(Debug, Clone)]
pub struct IterationNode {
    pub iteratable_name: String,
    pub block: BlockNode,
}

#[derive(Debug, Clone)]
pub struct ExpressionNode {
    pub root: AstNodeId,
    pub type_str: String,
}

// #[derive(Debug, Clone)]
// pub struct AstNode {
//     token: Option<Token>,
//     type_info: Option<TypeInfo>,
//     kind: AstNodeKind
// }

pub struct AstError {
    pub token: Token,
    pub expected: String,
}

impl AstError {
    pub fn new(token: Token, s: impl Into<String>) -> AstError {
        AstError {
            token,
            expected: s.into(),
        }
    }
}

type AstResult<T> = Result<T, AstError>;

#[derive(Debug, Clone, Default)]
pub struct TypeInfo {
    pub is_array: bool,
    pub type_str: String,
}

#[derive(Debug, Clone)]
pub enum AstNode {
    Program(Vec<AstNodeId>),
    Fn(FnData),
    VariableAssignment {
        variable_name: String,
        rexpression_id: AstNodeId
    },
    Expression(AstNodeId),
    Integer(String),
    Identifier {
        type_info: TypeInfo,
        name: String,
    },
    Operator(String),
    BinaryOperation {
        type_info: TypeInfo,
        operation: String,
        left_id: AstNodeId,
        right_id: AstNodeId,
    },
}

#[derive(Clone)]
struct CurrentFunctionContext {
    //function_node: FunctionNode,
    //iterators: Vec<(String, String)>,
    //iterators_count: u32,
    vars: HashMap<String, TypeInfo>,
    params: IndexMap<String, TypeInfo>,
    iterators: Vec<TypeInfo>,
}

#[derive(Clone)]
pub struct AstBuilder {
    ti: usize,
    pub tokenizer: Tokenizer,
    pub tokens: Vec<Token>,
    pub root_ids: Vec<AstNodeId>,
    pub records: HashMap<String, RecordNode>,
    pub ast_arena: Arena::<AstNode>,
    current_function_context: Option<CurrentFunctionContext>,
}

impl AstBuilder {
    pub fn new(tokenizer: &Tokenizer) -> AstBuilder {
        AstBuilder {
            ti: 0,
            tokenizer: tokenizer.clone(),
            tokens: tokenizer.tokens.clone(),
            root_ids: vec![],
            records: HashMap::new(),
            ast_arena: Arena::<AstNode>::new(),
            current_function_context: None,
        }
    }

    pub fn need_node(&self, id: &AstNodeId) -> &AstNode {
        self.ast_arena.get(*id).unwrap()
    }

    pub fn need_node_mut(&mut self, id: &AstNodeId) -> &mut AstNode {
        self.ast_arena.get_mut(*id).unwrap()
    }

    fn ctx(&mut self) -> &mut CurrentFunctionContext {
        self.current_function_context.as_mut().unwrap()
    }

    fn peek_next(&self) -> &Token {
        if self.ti < self.tokens.len() {
            &self.tokens[self.ti]
        } else {
            panic!()
        }
    }

    fn peek_next_next(&self) -> &Token {
        if self.ti < self.tokens.len() - 1 {
            &self.tokens[self.ti + 1]
        } else {
            panic!()
        }
    }

    fn next(&mut self) -> &Token {
        if self.ti < self.tokens.len() {
            let t = &self.tokens[self.ti];
            self.ti += 1;
            return t;
        } else {
            todo!();
        }
    }

    fn need_next_identifier_token(&mut self) -> AstResult<&Token> {
        let token = self.next();
        if token.kind != TokenKind::Identifier {
            return Err(AstError::new(token.clone(), "expected identifier"));
        }

        return Ok(&token);
    }

    fn skip_line_end_tokens(&mut self) {
        while self.tokens[self.ti].kind == TokenKind::LineEnd {
            self.ti += 1;
        }
    }

    fn skip_expected(&mut self, check: &str) -> AstResult<()> {
        if self.ti < self.tokens.len() {
            let token = &self.tokens[self.ti];
            if token.value != check {
                return Err(AstError::new(token.clone(), check));
            }
            self.ti += 1;
            Ok(())
        } else {
            let token = Token {
                kind: TokenKind::Eof,
                char_index: self.tokenizer.chars.len() - 1,
                value: "eof".to_string(),
            };
            Err(AstError::new(token, check))
        }
    }

    fn try_skip(&mut self, check: &str) -> bool {
        if self.peek_next().is_eof() {
            todo!();
        };

        let t = self.peek_next();
        //if t.kind == TokenKind::Special && t.value == check {
        if t.value == check {
            self.skip_expected(check);
            true
        } else {
            false
        }
    }

    fn skip(&mut self) {
        if self.ti < self.tokens.len() {
            self.ti += 1;
        } else {
            todo!();
        }
    }

    pub fn parse_tokens_to_ast(&mut self) -> AstResult<()> {
        let mut top_nodes = vec![];

        loop {
            self.skip_line_end_tokens();

            let token = self.peek_next();
            if token.is_eof() {
                break;
            };

            match token.kind {
                TokenKind::Keyword => match token.value.as_str() {
                    "fn" => {
                        let function_node = self.parse_function()?;
                        let node = AstNode::Fn(function_node);
                        let id = self.ast_arena.alloc(node);
                        //println!("{id:?}");
                        top_nodes.push(id);
                    }
                    else_ => {
                        return Err(AstError::new(token.clone(), "unexpected token"));
                    }
                },
                _ => {
                    return Err(AstError::new(token.clone(), "unexpected token"));
                }
            }
        }

        //println!("{:?}", self.ast_arena);
        self.root_ids = top_nodes;

        Ok(())
    }

    fn parse_record(&mut self) -> AstResult<RecordNode> {
        self.skip_expected("rec")?;

        let record_name_token = self.need_next_identifier_token()?.clone();
        let record_name = record_name_token.value.clone();
        self.skip_expected("{")?;

        let mut fields = IndexMap::new();
        loop {
            self.skip_line_end_tokens();

            if self.try_skip("}") {
                break;
            }

            let field_name = self.need_next_identifier_token()?.value.clone();
            self.skip_expected(":")?;
            let type_info = self.parse_type_info(&["\n"])?;

            fields.insert(field_name, type_info);
        }

        let record_node = RecordNode {
            name: record_name.clone(),
            fields,
        };

        match self.records.entry(record_name) {
            Entry::Occupied(occupied_entry) => {
                return Err(AstError::new(record_name_token, "duplicate record identifier"));
            }
            Entry::Vacant(vacant_entry) => {
                vacant_entry.insert(record_node.clone());
            }
        }

        Ok(record_node)
    }

    fn parse_function(&mut self) -> AstResult<FnData> {
        self.skip_expected("fn")?;

        self.current_function_context = Some(CurrentFunctionContext {
            params: IndexMap::new(),
            vars: HashMap::new(),
            iterators: vec![],
        });

        let fn_name_token = self.next();
        if fn_name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(fn_name_token.clone(), "identifier expected"));
        }

        let fn_name = fn_name_token.value.clone();
        let fn_params = self.parse_function_declaration_params()?;
        let fn_return_type = self.parse_function_return_type()?;
        self.ctx().params = fn_params.clone();
        let fn_body = self.parse_function_body()?;
        //let vars = mem::take(&mut self.current_function_vars);
        let fn_vars = self.ctx().vars.clone();

        Ok(FnData {
            name: fn_name,
            params: fn_params,
            vars: fn_vars,
            body: fn_body,
            return_type: fn_return_type,
        })
    }

    fn parse_function_declaration_params(&mut self) -> AstResult<IndexMap<String, TypeInfo>> {
        if !self.try_skip("(") {
            return Ok(IndexMap::new());
        }

        let mut params = IndexMap::new();
        loop {
            if self.try_skip(")") {
                break;
            }

            let (name, type_info) = self.parse_function_declaration_param(&[",", ")"])?;
            params.insert(name, type_info);

            self.try_skip(",");
        }

        Ok(params)
    }

    fn parse_function_return_type(&mut self) -> AstResult<TypeInfo> {
        self.skip_line_end_tokens();

        if self.peek_next().value == "begin" {
            return Ok(TypeInfo {
                is_array: false,
                type_str: String::new(),
            });
        }

        let return_type = self.parse_type_info(&["begin"])?;

        Ok(return_type)
    }

    fn parse_function_declaration_param(&mut self, terminators: &[&str]) -> AstResult<(String, TypeInfo)> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(name_token, "identifier expected"));
        }
        let var_name = name_token.value.clone();
        self.skip_expected(":")?;

        let mut is_array: bool;
        let mut type_str: String;
        if self.peek_next().value == "[" {
            is_array = true;
            self.skip_expected("[")?;
            type_str = self.next().value.clone();
            self.skip_expected("]")?;
        } else {
            is_array = false;
            type_str = self.next().value.clone();
        }

        let type_info = TypeInfo {
            is_array,
            type_str: type_str.clone(),
        };

        Ok((var_name, type_info))
    }

    fn parse_function_body(&mut self) -> AstResult<BlockNode> {
        self.skip_line_end_tokens();
        self.skip_expected("begin")?;
        let mut statements = vec![];
        loop {
            self.skip_line_end_tokens();

            let t = self.peek_next();

            if t.kind == TokenKind::Keyword && t.value == "end" {
                self.skip_expected("end")?;
                break;
            }

            let statement = self.parse_statement()?;
            statements.push(statement);
        }

        Ok(BlockNode { statement_id_list: statements })
    }

    fn parse_block(&mut self) -> AstResult<BlockNode> {
        self.skip_expected("{")?;
        let mut statements = vec![];
        loop {
            self.skip_line_end_tokens();

            let t = self.peek_next();
            if t.kind == TokenKind::SpecialSymbol && t.value == "}" {
                self.skip_expected("}")?;
                break;
            }

            let statement = self.parse_statement()?;
            statements.push(statement);
        }

        Ok(BlockNode { statement_id_list: statements })
    }

    fn parse_statement(&mut self) -> AstResult<AstNodeId> {
        let token = self.peek_next();
        match token.kind {
            TokenKind::Identifier => {
                match self.peek_next_next().value.as_str() {
                    "=" => {
                        let id = self.parse_variable_assignment()?;
                        return Ok(id);
                    }
                    _ => panic!()
                }
            }

            _ => todo!(),
        }
    }

    fn parse_variable_assignment(&mut self) -> AstResult<AstNodeId> {
        let name_token = self.need_next_identifier_token()?;
        let var_name = name_token.value.clone();
        self.skip_expected("=")?;

        let expression_node_id = self.parse_expression_until(&["\n"])?;
        // let type_str = expression.type_str.clone();

        // let info = match self.ctx().vars.entry(var_name.clone()) {
        //     Entry::Occupied(occupied) => {
        //         let info = occupied.into_mut();
        //         // if info.type_str != type_str {
        //         //     let s = format!(
        //         //         "variable assignment with different type than first assignment\nwas {}, became {}",
        //         //         info.type_str.clone(),
        //         //         type_str.clone()
        //         //     );
        //         //     return Err(ae(&name_token, s));
        //         // }

        //         // todo:
        //         // if info.is_array != is_array {
        //         //     let s = format!(
        //         //         "variable assignment with different type than first assignment\nwas {}, became {}",
        //         //         info.type_str.clone(), type_str.clone()
        //         //     );
        //         //     return Err(ae(&name_token, s));
        //         // }

        //         info
        //     }
        //     Entry::Vacant(vacant) => {
        //         let info = TypeInfo {
        //             is_array: false,
        //             type_str: type_str.clone(),
        //         };
        //         vacant.insert(info)
        //     }
        // };

        let node = AstNode::VariableAssignment { 
            variable_name: var_name, 
            rexpression_id: expression_node_id 
        };
        let id = self.ast_arena.alloc(node);

        Ok(id)
    }

    fn parse_type_info(&mut self, terminators: &[&str]) -> AstResult<TypeInfo> {
        let mut is_array = false;
        let mut type_str: String;
        if self.peek_next().value == "[" {
            is_array = true;
            self.skip_expected("[")?;
            type_str = self.next().value.clone();
            self.skip_expected("]")?;
        } else {
            is_array = false;
            type_str = self.next().value.clone();
        }

        Ok(TypeInfo { is_array, type_str })
    }

    fn parse_variable_declaration(&mut self) -> AstResult<VariableDeclarationNode> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(name_token, "identifier expected"));
        }
        let var_name = name_token.value.clone();
        self.skip_expected(":")?;

        let type_info = self.parse_type_info(&["\n", ""])?;

        //let info = self.current_function_context.as_mut().unwrap().vars.get(&var_name);
        let info = match self.ctx().vars.entry(var_name.clone()) {
            Entry::Occupied(occupied) => {
                let info = occupied.into_mut();
                let s = format!("variable already defined {}", var_name.clone());
                return Err(ae(&name_token, s));
            }
            Entry::Vacant(vacant) => vacant.insert(type_info),
        };

        Ok(VariableDeclarationNode { name: var_name.clone() })
    }

    fn parse_expression_until(&mut self, terminators: &[&str]) -> AstResult<AstNodeId> {
        let mut ast_nodes = vec![];
        loop {
            let t = self.peek_next();
            if t.kind == TokenKind::SpecialSymbol {
                if terminators.contains(&t.value.as_str()) {
                    break;
                }
            }

            let token = self.next().clone();
            let value = token.value.clone();
            match token.kind {
                TokenKind::LineEnd => {
                    break;
                }
                TokenKind::Identifier => {
                    let node = AstNode::Identifier { type_info: TypeInfo::default(), name: value };
                    ast_nodes.push(node);
                }
                TokenKind::SpecialSymbol => {
                    let node = AstNode::Operator(value);
                    ast_nodes.push(node);
                }
                TokenKind::IntegerLiteral => {
                    let node = AstNode::Integer(value);
                    ast_nodes.push(node);
                }
                _ => (),
            }
        }

        let mut root = TreeNode {
            ast_node: ast_nodes[0].clone(),
            childs: vec![],
        };

        let mut index = 1;
        loop {
            if index < ast_nodes.len() {
                root = self.ast_node_list_to_tree_node(&ast_nodes, &mut index, root, 0);
                //println!("{:?}", root);
            } else {
                break;
            }
        }

        let node_id = self.tree_node_to_ast_node(&root)?;
        // let expression_node = AstNode::Expression(node_id);
        // let expression_node_id = self.ast_arena.alloc(expression_node);
        //Ok(expression_node_id)
        
        Ok(node_id)
    }

    fn ast_node_list_to_tree_node(
        &mut self,
        ast_node_list: &Vec<AstNode>,
        next_ast_node_index: &mut usize,
        tree_node: TreeNode,
        priority: usize,
    ) -> TreeNode {
        let mut tree_node = tree_node;
        let mut priority = priority;
        loop {
            if *next_ast_node_index >= ast_node_list.len() {
                break;
            }

            let next_ast_node = &ast_node_list[*next_ast_node_index];

            match next_ast_node {
                AstNode::Operator(operator) => {
                    let next_priority = *self.tokenizer.priorities.get(operator).unwrap();

                    if next_priority < priority {
                        break;
                    }

                    let next_next_ast_node = &ast_node_list[*next_ast_node_index + 1];
                    let mut next_tree_node = TreeNode {
                        ast_node: next_next_ast_node.clone(),
                        childs: vec![],
                    };

                    *next_ast_node_index += 2;
                    next_tree_node = self.ast_node_list_to_tree_node(
                        ast_node_list,
                        next_ast_node_index,
                        next_tree_node,
                        next_priority,
                    );

                    let mut childs = vec![];
                    childs.push(tree_node);
                    childs.push(next_tree_node);
                    tree_node = TreeNode {
                        ast_node: next_ast_node.clone(),
                        childs,
                    }
                }
                _ => {
                    //println!("{:?}", next_op);
                    todo!();
                }
            }
        }

        tree_node
    }

    fn tree_node_to_ast_node(&mut self, tree_node: &TreeNode) -> AstResult<AstNodeId> {
        let id = match &tree_node.ast_node {
            AstNode::Identifier{..} => {
                let id = self.ast_arena.alloc(tree_node.ast_node.clone());
                return Ok(id);
            }

            AstNode::Integer(integer_literal_str) => {
                let id = self.ast_arena.alloc(tree_node.ast_node.clone());
                return Ok(id);
            }

            AstNode::Operator(o) => {
                let left = self.tree_node_to_ast_node(&tree_node.childs[0])?;
                let right = self.tree_node_to_ast_node(&tree_node.childs[1])?;
                let node = AstNode::BinaryOperation {
                    type_info: TypeInfo::default(),
                    operation: o.clone(),
                    left_id: left, 
                    right_id: right
                };
                let id = self.ast_arena.alloc(node);
                return Ok(id);
            }

            else_ => {
                panic!();
            }
        };
        id
    }
}

#[derive(Debug)]
struct TreeNode {
    ast_node: AstNode,
    childs: Vec<TreeNode>,
}

fn ae(t: &Token, s: impl Into<String>) -> AstError {
    AstError::new(t.clone(), s.into())
}
