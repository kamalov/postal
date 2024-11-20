#![allow(warnings)]
use std::collections::hash_map::Entry;
use std::collections::{HashMap, HashSet};
use std::env::var;
use std::fmt;
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::ops::Index;
use std::slice::Iter;
use std::{array, default, fs, iter, mem};

use indexmap::IndexMap;
use to_vec::ToVec;

use crate::stage01_tokenizer::*;

pub struct AstError {
    pub token: Token,
    pub expected: String,
}

impl AstError {
    pub fn new(token: &Token, s: impl Into<String>) -> AstError {
        AstError {
            token: token.clone(),
            expected: s.into(),
        }
    }
}

type AstResult<T> = Result<T, AstError>;

#[derive(Debug, Clone)]
pub struct Function {
    pub decl: FnDeclaration,
    pub vars: IndexMap<String, TypeInfo>,
    pub body: Block,
}

#[derive(Debug, Clone)]
pub struct FnDeclaration {
    pub name: String,
    pub params: IndexMap<String, TypeInfo>,
    pub return_type: Option<TypeInfo>,
}

#[derive(Debug, Clone)]
pub struct Record {
    pub name: String,
    pub fields: IndexMap<String, TypeInfo>,
}

#[derive(Debug, Clone)]
pub struct Block {
    pub statements: Vec<StatementNode>,
}

#[derive(Debug, Clone)]
pub struct VariableAssignment {
    pub token: Token,
    pub name: String,
    pub rvalue_expression: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct Assignment {
    pub token: Token,
    pub lvalue: ExpressionNode,
    pub rvalue: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct FunctionCall {
    pub token: Token,
    pub name: String,
    pub params_group: GroupNode,
    pub params_types: Vec<TypeInfo>,
}

#[derive(Debug, Clone)]
pub struct GroupNode {
    pub token: Token,
    pub expressions: Vec<ExpressionNode>,
}

#[derive(Debug, Clone)]
pub struct ArrayBracketsNode {
    pub expression: Box<ExpressionNode>,
}

#[derive(Debug, Clone)]
pub struct IdentifierNode {
    pub token: Token,
    pub value: String,
}

#[derive(Debug, Clone)]
pub struct OperatorNode {
    token: Token,
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
pub struct BinaryOperationNode {
    pub token: Token,
    pub operation: OperatorNode,
    pub left: Box<ExpressionNode>,
    pub right: Box<ExpressionNode>,
}

#[derive(Debug, Clone)]
pub struct IfStatement {
    pub if_blocks: Vec<(ExpressionNode, Block)>,
    pub else_block: Option<Block>,
}

#[derive(Debug, Clone)]
pub struct LoopStatement {
    pub block: Block,
}

#[derive(Debug, Clone)]
pub struct IterationStatement {
    pub token: Token,
    pub iteratable_name: String,
    pub block: Block,
}

#[derive(Debug, Clone)]
pub enum ExpressionNode {
    FunctionCall(FunctionCall),
    IntegerLiteral(String),
    RealLiteral(String),
    StringLiteral(String),
    Identifier(IdentifierNode),
    Operator(OperatorNode),
    BinaryOperation(BinaryOperationNode),
    Group(GroupNode),
    ArrayBrackets(ArrayBracketsNode),
    ArrayItemAccess {
        token: Token,
        array_name: String,
        access_expression: Box<ExpressionNode>,
    },
}

#[derive(Debug, Clone)]
pub enum StatementNode {
    If(IfStatement),
    Loop(LoopStatement),
    Iteration((Token, String, Block)),
    Break(),
    Return((Token, ExpressionNode)),
    FunctionCall(FunctionCall),
    VariableAssignment(VariableAssignment),
    Assignment(Assignment),
    VariableDeclaration((Token, String, TypeInfo)),
    Comment(String),
}

#[derive(Debug, Clone)]
pub enum RootNode {
    Function(Function),
    Record(Record),
    Comment(String),
}

#[derive(Debug, Clone, Default, PartialEq)]
pub struct TypeInfo {
    pub is_array: bool,
    pub type_str: String,
}

impl fmt::Display for TypeInfo {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let array_qualifier = if self.is_array { "[]" } else { "" };
        write!(f, "{}{}", self.type_str, array_qualifier)
    }
}

#[derive(Clone)]
pub struct AstBuilder {
    current_token_index: usize,
    pub root_nodes: Vec<RootNode>,
    pub tokenizer: Tokenizer,
    pub tokens: Vec<Token>,
    pub records: HashMap<String, Record>,
    pub fn_declarations: HashMap<String, FnDeclaration>,
}

impl AstBuilder {
    pub fn new(tokenizer: &Tokenizer) -> AstBuilder {
        AstBuilder {
            current_token_index: 0,
            root_nodes: vec![],
            tokenizer: tokenizer.clone(),
            tokens: tokenizer.tokens.clone(),
            records: HashMap::new(),
            fn_declarations: HashMap::new(),
        }
    }

    fn peek_next(&self) -> &Token {
        if self.current_token_index < self.tokens.len() {
            &self.tokens[self.current_token_index]
        } else {
            panic!()
        }
    }

    fn peek_next_next(&self) -> &Token {
        if self.current_token_index < self.tokens.len() - 1 {
            &self.tokens[self.current_token_index + 1]
        } else {
            panic!()
        }
    }

    fn next(&mut self) -> &Token {
        if self.current_token_index < self.tokens.len() {
            let t = &self.tokens[self.current_token_index];
            self.current_token_index += 1;
            return t;
        } else {
            todo!();
        }
    }

    fn need_next_identifier_token(&mut self) -> AstResult<&Token> {
        let token = self.next();
        if token.kind != TokenKind::Identifier {
            return Err(AstError::new(token, "expected identifier"));
        }

        return Ok(&token);
    }

    fn skip_line_end_tokens(&mut self) {
        while self.tokens[self.current_token_index].kind == TokenKind::LineEnd {
            self.current_token_index += 1;
        }
    }

    fn skip_expected(&mut self, check: &str) -> AstResult<()> {
        if self.current_token_index < self.tokens.len() {
            let token = &self.tokens[self.current_token_index];
            if token.value != check {
                return Err(AstError::new(&token, check));
            }
            self.current_token_index += 1;
            Ok(())
        } else {
            let token = Token {
                kind: TokenKind::Eof,
                char_index: self.tokenizer.chars.len() - 1,
                value: "eof".to_string(),
            };
            Err(AstError::new(&token, check))
        }
    }

    fn try_skip(&mut self, check: &str) -> bool {
        if self.peek_next().is_eof() {
            todo!();
        };

        let t = self.peek_next();
        if t.value == check {
            self.skip_expected(check);
            true
        } else {
            false
        }
    }

    fn skip(&mut self) {
        if self.current_token_index < self.tokens.len() {
            self.current_token_index += 1;
        } else {
            todo!();
        }
    }

    pub fn parse_tokens_to_ast(&mut self) -> AstResult<()> {
        let mut root_nodes = vec![];

        loop {
            self.skip_line_end_tokens();

            let token = self.peek_next();
            if token.is_eof() {
                break;
            };

            match token.kind {
                TokenKind::Keyword => match token.value.as_str() {
                    "fn" => {
                        let fun = self.parse_function()?;
                        root_nodes.push(RootNode::Function(fun));
                    }
                    "rec" => {
                        let record = self.parse_record()?;
                        root_nodes.push(RootNode::Record(record));
                    }
                    else_ => {
                        return Err(AstError::new(&token, "unexpected token"));
                    }
                },
                TokenKind::Comment => {
                    let value = token.value.clone();
                    self.skip();
                    root_nodes.push(RootNode::Comment(value));
                }
                _ => {
                    return Err(AstError::new(&token, "unexpected token"));
                }
            }
        }

        self.root_nodes = root_nodes;
        Ok(())
    }

    fn parse_type_info_until(&mut self, terminators: &[&str]) -> AstResult<TypeInfo> {
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

    fn parse_record(&mut self) -> AstResult<Record> {
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
            let type_info = self.parse_type_info_until(&["\n"])?;

            fields.insert(field_name, type_info);
        }

        let record_node = Record {
            name: record_name.clone(),
            fields,
        };

        match self.records.entry(record_name) {
            Entry::Occupied(occupied_entry) => {
                return Err(AstError::new(&record_name_token, "duplicate record identifier"));
            }
            Entry::Vacant(vacant_entry) => {
                vacant_entry.insert(record_node.clone());
            }
        }

        Ok(record_node)
    }

    fn parse_function(&mut self) -> AstResult<Function> {
        self.skip_expected("fn")?;

        let fn_name_token = self.next().clone();
        if fn_name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(&fn_name_token, "identifier expected"));
        }

        let fn_name = fn_name_token.value.clone();
        let fn_params = self.parse_function_declaration_params()?;
        let fn_return_type = self.parse_function_return_type()?;

        let fn_declaration = FnDeclaration {
            name: fn_name.clone(),
            params: fn_params,
            return_type: fn_return_type,
        };

        if self.fn_declarations.contains_key(&fn_name) {
            return Err(AstError::new(&fn_name_token, "duplicate function name"));
        } else {
            self.fn_declarations.insert(fn_name, fn_declaration.clone());
        }

        let fn_body = self.parse_function_body()?;

        Ok(Function {
            decl: fn_declaration,
            vars: IndexMap::new(),
            body: fn_body,
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

    fn parse_function_declaration_param(&mut self, terminators: &[&str]) -> AstResult<(String, TypeInfo)> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(&name_token, "identifier expected"));
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

    fn parse_function_return_type(&mut self) -> AstResult<Option<TypeInfo>> {
        self.skip_line_end_tokens();

        if self.peek_next().value == "begin" {
            return Ok(None);
        }

        let return_type = self.parse_type_info_until(&["begin"])?;

        Ok(Some(return_type))
    }

    fn parse_function_body(&mut self) -> AstResult<Block> {
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

        Ok(Block { statements })
    }

    fn parse_function_call(&mut self) -> AstResult<FunctionCall> {
        let token = self.next().clone();
        let name = token.value.clone();
        let params_group = self.parse_group()?;
        Ok(FunctionCall {
            token,
            name,
            params_group,
            params_types: vec![]
        })
    }

    fn parse_block(&mut self) -> AstResult<Block> {
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

        Ok(Block { statements })
    }

    fn parse_statement(&mut self) -> AstResult<StatementNode> {
        let token = self.peek_next().clone();
        match token.kind {
            TokenKind::Identifier => {
                match self.peek_next_next().value.as_str() {
                    "(" => {
                        let f = self.parse_function_call()?;
                        return Ok(StatementNode::FunctionCall(f));
                    }

                    "=" => {
                        let statement = self.parse_variable_assignment_statement()?;
                        return Ok(StatementNode::VariableAssignment(statement));
                    }

                    ":" => {
                        let statement = self.parse_variable_declaration_statement()?;
                        return Ok(StatementNode::VariableDeclaration(statement));
                    }

                    ">>" => {
                        let statement = self.parse_iteration_statement()?;
                        return Ok(StatementNode::Iteration(statement));
                    }

                    _ => {}
                }

                let left_side_expression = self.parse_expression_until(&["=", "\n"])?;
                self.skip_expected("=");
                let right_side_expression = self.parse_expression_until(&["\n"])?;
                return Ok(StatementNode::Assignment(Assignment {
                    token: token.clone(),
                    lvalue: left_side_expression,
                    rvalue: right_side_expression,
                }));
            }

            TokenKind::Keyword => match token.value.as_str() {
                "if" => {
                    let f = self.parse_if_statement()?;
                    return Ok(StatementNode::If(f));
                }
                "loop" => {
                    let f = self.parse_loop_statement()?;
                    return Ok(StatementNode::Loop(f));
                }
                "break" => {
                    self.skip_expected("break")?;
                    return Ok(StatementNode::Break());
                }
                "ret" => {
                    self.skip_expected("ret")?;
                    let expression_node = self.parse_expression_until(&["\n"])?;
                    return Ok(StatementNode::Return((token.clone(), expression_node)));
                }
                panic_ => {
                    panic!();
                }
            },

            TokenKind::Comment => {
                let value = token.value.clone();
                self.skip();
                Ok(StatementNode::Comment(value))
            }

            _ => {
                println!("{token:?}");
                panic!();
            }
        }
    }

    fn parse_if_statement(&mut self) -> AstResult<IfStatement> {
        self.skip_expected("if")?;

        let mut if_blocks: Vec<(ExpressionNode, Block)> = Vec::new();
        let mut else_block = None;

        let condition = self.parse_expression_until(&["{"])?;
        let block = self.parse_block()?;
        if_blocks.push((condition, block));

        loop {
            if self.try_skip("else") {
                if self.try_skip("if") {
                    let condition = self.parse_expression_until(&["{"])?;
                    let block = self.parse_block()?;
                    if_blocks.push((condition, block));
                } else {
                    else_block = Some(self.parse_block()?);
                    break;
                }
            } else {
                break;
            }
        }

        Ok(IfStatement { if_blocks, else_block })
    }

    fn parse_loop_statement(&mut self) -> AstResult<LoopStatement> {
        self.skip_expected("loop")?;

        let block = self.parse_block()?;
        Ok(LoopStatement { block })
    }

    fn parse_iteration_statement(&mut self) -> AstResult<(Token, String, Block)> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(&name_token, "identifier expected"));
        }
        let iteratable_name = name_token.value.clone();

        self.skip_expected(">>")?;
        let block = self.parse_block()?;

        Ok((name_token.clone(), iteratable_name, block))
    }

    fn parse_variable_assignment_statement(&mut self) -> AstResult<VariableAssignment> {
        let name_token = self.need_next_identifier_token()?.clone();
        let var_name = name_token.value.clone();
        self.skip_expected("=")?;

        let expression = self.parse_expression_until(&["\n"])?;

        Ok(VariableAssignment {
            token: name_token.clone(),
            name: var_name.clone(),
            rvalue_expression: expression,
        })
    }

    fn parse_variable_declaration_statement(&mut self) -> AstResult<(Token, String, TypeInfo)> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(&name_token, "identifier expected"));
        }
        let var_name = name_token.value.clone();
        self.skip_expected(":")?;

        let type_info = self.parse_type_info_until(&["\n", ""])?;

        Ok((name_token.clone(), var_name, type_info))
    }

    fn parse_group(&mut self) -> AstResult<GroupNode> {
        let token = self.peek_next().clone();
        self.skip_expected("(")?;
        let mut expressions = vec![];
        loop {
            if self.try_skip(")") {
                break;
            }

            let expression = self.parse_expression_until(&[",", ")"])?;
            expressions.push(expression);

            self.try_skip(",");
        }

        Ok(GroupNode { token, expressions })
    }

    fn parse_array_brackets(&mut self) -> AstResult<ArrayBracketsNode> {
        let token = self.peek_next().clone();
        self.skip_expected("[")?;
        let expression_node = self.parse_expression_until(&[",", "]"])?;
        self.skip_expected("]")?;

        Ok(ArrayBracketsNode {
            expression: Box::new(expression_node),
        })
    }

    fn parse_expression_until(&mut self, terminators: &[&str]) -> AstResult<ExpressionNode> {
        let mut ast_nodes = vec![];
        loop {
            let t = self.peek_next();
            if t.kind == TokenKind::SpecialSymbol {
                if terminators.contains(&t.value.as_str()) {
                    break;
                }

                if t.value == "(" {
                    let group_node = self.parse_group()?;
                    let node = ExpressionNode::Group(group_node);
                    ast_nodes.push(node);
                    continue;
                }

                if t.value == "[" {
                    let array_brackets_node = self.parse_array_brackets()?;
                    let node = ExpressionNode::ArrayBrackets(array_brackets_node);
                    ast_nodes.push(node);
                    continue;
                }
            }

            let token = self.next();
            match token.kind {
                TokenKind::LineEnd => {
                    break;
                }
                TokenKind::Identifier => {
                    let node = ExpressionNode::Identifier(IdentifierNode {
                        token: token.clone(),
                        value: token.value.clone(),
                    });
                    ast_nodes.push(node);
                }
                TokenKind::SpecialSymbol => {
                    let node = ExpressionNode::Operator(OperatorNode {
                        token: token.clone(),
                        value: token.value.clone(),
                    });
                    ast_nodes.push(node);
                }
                TokenKind::IntegerLiteral => {
                    let node = ExpressionNode::IntegerLiteral(token.value.clone());
                    ast_nodes.push(node);
                }
                TokenKind::FloatLiteral => {
                    let node = ExpressionNode::RealLiteral(token.value.clone());
                    ast_nodes.push(node);
                }
                TokenKind::String => {
                    let node = ExpressionNode::StringLiteral(token.value.clone());
                    ast_nodes.push(node);
                }
                _ => (),
            }
        }

        let mut root = TreeNode {
            expression_node: ast_nodes[0].clone(),
            childs: vec![],
        };

        let mut index = 1;
        loop {
            if index < ast_nodes.len() {
                root = self.expression_node_list_to_tree_node(&ast_nodes, &mut index, root, 0);
                //println!("{:?}", root);
            } else {
                break;
            }
        }

        let node = self.tree_node_to_expression_node(&root)?;
        Ok(node)
    }

    fn expression_node_list_to_tree_node(
        &mut self,
        ast_node_list: &Vec<ExpressionNode>,
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
                ExpressionNode::Operator(operator) => {
                    let next_priority = *self.tokenizer.priorities.get(&operator.value).unwrap();

                    if next_priority < priority {
                        break;
                    }

                    let next_next_ast_node = &ast_node_list[*next_ast_node_index + 1];
                    let mut next_tree_node = TreeNode {
                        expression_node: next_next_ast_node.clone(),
                        childs: vec![],
                    };

                    *next_ast_node_index += 2;
                    next_tree_node = self.expression_node_list_to_tree_node(
                        ast_node_list,
                        next_ast_node_index,
                        next_tree_node,
                        next_priority,
                    );

                    let mut childs = vec![];
                    childs.push(tree_node);
                    childs.push(next_tree_node);
                    tree_node = TreeNode {
                        expression_node: next_ast_node.clone(),
                        childs,
                    }
                }

                ExpressionNode::Group(group_node) => match &tree_node.expression_node {
                    ExpressionNode::Identifier(identifier_node) => {
                        let node = ExpressionNode::FunctionCall(FunctionCall {
                            token: identifier_node.token.clone(),
                            name: identifier_node.value.clone(),
                            params_group: group_node.clone(),
                            params_types: vec![]
                        });
                        tree_node.expression_node = node;
                        *next_ast_node_index += 1;
                        break;
                    }

                    else_ => {
                        println!("{:?}", tree_node.expression_node);
                        panic!()
                    }
                },

                ExpressionNode::ArrayBrackets(array_brackets_node) => match &tree_node.expression_node {
                    ExpressionNode::Identifier(identifier_node) => {
                        let node = ExpressionNode::ArrayItemAccess {
                            token: identifier_node.token.clone(),
                            array_name: identifier_node.value.clone(),
                            access_expression: array_brackets_node.expression.clone(),
                        };
                        tree_node.expression_node = node;
                        *next_ast_node_index += 1;
                        break;
                    }

                    else_ => {
                        println!("{:?}", tree_node.expression_node);
                        panic!()
                    }
                },

                _ => {
                    //println!("{:?}", next_op);
                    todo!();
                }
            }
        }

        tree_node
    }

    fn tree_node_to_expression_node(&self, tree_node: &TreeNode) -> AstResult<ExpressionNode> {
        match &tree_node.expression_node {
            ExpressionNode::Operator(op) => {
                let left = self.tree_node_to_expression_node(&tree_node.childs[0])?;
                let right = self.tree_node_to_expression_node(&tree_node.childs[1])?;
                let node = ExpressionNode::BinaryOperation(BinaryOperationNode {
                    token: op.token.clone(),
                    operation: op.clone(),
                    left: Box::new(left),
                    right: Box::new(right),
                });
                return Ok(node);
            }

            else_ => {
                //assert!(false, "can't detect type");
                return Ok(tree_node.expression_node.clone());
            }
        }
    }
}

#[derive(Debug)]
struct TreeNode {
    expression_node: ExpressionNode,
    childs: Vec<TreeNode>,
}
