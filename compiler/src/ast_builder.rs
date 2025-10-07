use std::collections::hash_map::Entry;
use std::collections::{HashMap, HashSet};
use std::fmt::{Binary, Write};
use std::mem::uninitialized;
use std::path::Iter;
use std::{fmt, vec};

use id_arena::{Arena, Id};
use indexmap::{IndexMap, IndexSet};

use crate::compiler::*;
use crate::tokenizer::*;
use crate::type_info::*;
use crate::utils::*;

pub struct AstError {
    pub token: Token,
    pub message: String,
}

impl AstError {
    pub fn new(token: Token, s: impl Into<String>) -> AstError {
        AstError {
            token: token,
            message: s.into(),
        }
    }
}

type AstResult<T> = Result<T, AstError>;

#[derive(Debug, Clone)]
pub enum RootNode {
    Function(Function),
    Record(Record),
    Comment(String),
}

#[derive(Debug, Clone)]
pub struct Function {
    pub is_external: bool,
    pub declaration: FunctionDeclaration,
    pub vars: IndexMap<String, TypeInfo>,
    pub body: Block,
}

#[derive(Debug, Clone)]
pub struct FunctionDeclaration {
    pub token: Token,
    pub name: String,
    pub generic_params: IndexSet<String>,
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
    pub statements: Vec<Statement>,
}

#[derive(Debug, Clone)]
pub struct VariableAssignment {
    pub token: Token,
    pub name: String,
    pub rvalue_expression: Expression,
}

#[derive(Debug, Clone)]
pub struct Assignment {
    pub token: Token,
    pub lvalue: Expression,
    pub rvalue: Expression,
}

#[derive(Debug, Clone)]
pub struct FunctionCall {
    pub name: String,
    pub params: Vec<Expression>,
}

#[derive(Debug, Clone)]
pub struct IfElseStatement {
    pub if_blocks: Vec<(Expression, Block)>,
    pub else_block: Option<Block>,
}

#[derive(Debug, Clone)]
pub struct LoopStatement {
    pub block: Block,
}

#[derive(Debug, Clone)]
pub struct IterationStatement {
    pub token: Token,
    pub iterable_name: String,
    pub block: Block,
}

#[derive(Debug, Clone)]
pub struct ForStatement {
    pub iterable_expression: Expression,
    pub block: Block,
}

#[derive(Debug, Clone)]
pub enum ExpressionKind {
    FunctionCall(FunctionCall),
    IntegerLiteral(String),
    RealLiteral(String),
    BooleanLiteral(String),
    StringLiteral(String),
    Identifier(String),
    Group(Vec<Expression>),
    ObjectInitializer(String),
    ArrayInitializer(String),
    HashMapInitializer(String, String),
    UnaryOperation { operator: String, expr: Expression },
    BinaryOperation { operator: String, left: Expression, right: Expression },
    ArrayItemAccess { array_expression: Expression, access_expression: Expression },

    // todo (low priority): tmp operators, used only on parsing stage, not presented in final tree, needs refactoring
    Operator(String),
    ArrayBrackets(Expression),
    ObjectLiteral,
}

#[derive(Debug, Clone)]
pub struct Expression {
    pub token: Token,
    pub kind: Box<ExpressionKind>,
    pub type_info: Option<TypeInfo>,
}

impl Expression {
    pub fn new(token: Token, kind: ExpressionKind) -> Expression {
        Expression {
            token: token,
            kind: Box::new(kind),
            type_info: None,
        }
    }

    pub fn is_op(&self) -> bool {
        match &*self.kind {
            ExpressionKind::Operator(op) => true,
            _ => false,
        }
    }

    pub fn get_op(&self) -> String {
        match &*self.kind {
            ExpressionKind::Operator(op) => op.to_string(),
            _ => panic!(),
        }
    }
}

#[derive(Debug, Clone)]
pub enum Statement {
    If(IfElseStatement),
    Loop(LoopStatement),
    Iteration(IterationStatement),
    For(ForStatement),
    Break(),
    Continue(),
    Return(Option<Expression>),
    VariableAssignment(VariableAssignment),
    Assignment(Assignment),
    Expression(Expression),
    VariableDeclaration((Token, String, TypeInfo)),
    Comment(String),
}

#[derive(PartialEq, Eq, Hash, Clone)]
pub enum OpPriorityKind {
    Unary,
    Binary,
}

pub struct AstBuilder<'compiler> {
    c: &'compiler Compiler,
    tokens: &'compiler Vec<Token>,
    current_token_index: usize,
    priorities: HashMap<(String, OpPriorityKind), usize>,
    root_nodes: Vec<RootNode>,
    records: HashMap<String, Record>,
}

#[derive(Debug, Clone)]
pub struct Ast {
    pub root_nodes: Vec<RootNode>,
    pub records: HashMap<String, Record>,
}

pub fn build_ast(compiler: &Compiler) -> AstResult<Ast> {
    let mut builder = AstBuilder::new(compiler);
    builder.parse_tokens_to_ast()?;
    return Ok(Ast { root_nodes: builder.root_nodes, records: builder.records });
}

impl<'compiler> AstBuilder<'compiler> {
    pub fn new(compiler: &'compiler Compiler) -> AstBuilder<'compiler> {
        AstBuilder {
            c: compiler,
            tokens: &compiler.tokens,
            current_token_index: 0,
            root_nodes: vec![],
            records: HashMap::new(),
            priorities: create_priorities(),
        }
    }

    /// token iteration helpers
    
    fn peek_next_token(&self) -> Token {
        self.tokens[self.current_token_index]
    }

    fn peek_next_next_token(&self) -> Token {
        self.tokens[self.current_token_index + 1]
    }

    fn next_token(&mut self) -> Token {
        let token = self.tokens[self.current_token_index];
        self.current_token_index += 1;
        token
    }

    fn need_next_identifier_token(&mut self) -> AstResult<Token> {
        let token = self.next_token();
        if token.kind != TokenKind::Identifier {
            return Err(AstError::new(token, "expected identifier"));
        }

        return Ok(token);
    }

    fn skip_line_end_tokens(&mut self) {
        while self.tokens[self.current_token_index].kind == TokenKind::LineEnd {
            self.current_token_index += 1;
        }
    }

    fn skip_expected_token(&mut self, check: &str) -> AstResult<()> {
        let token = self.next_token();
        if self.c.get_token_value(&token) != check {
            return Err(AstError::new(token, "expected ".to_string() + check));
        }
        Ok(())
    }

    fn check_next_non_string_token_value(&mut self, check: &str) -> bool {
        let token = self.peek_next_token();
        let value = self.c.get_token_value(&token);
        return token.kind != TokenKind::StringLiteral && self.c.get_token_value(&token) == check;
    }

    fn check_next_non_string_token_values(&mut self, checks: &[&str]) -> bool {
        for check in checks {
            if self.check_next_non_string_token_value(check) {
                return true;
            }
        }
        return false;
    }

    fn try_skip_non_string_token(&mut self, check: &str) -> bool {
        let checked = self.check_next_non_string_token_value(check);
        if checked {
            self.skip_token();
        }
        return checked;
    }

    fn skip_token(&mut self) {
        self.current_token_index += 1;
    }

    /// parsing 
    
    pub fn parse_tokens_to_ast(&mut self) -> AstResult<()> {
        let mut root_nodes = vec![];

        loop {
            self.skip_line_end_tokens();

            let token = self.peek_next_token();
            if token.kind == TokenKind::Eof {
                break;
            };

            match token.kind {
                TokenKind::Keyword => match self.c.get_token_value(&token) {
                    "function" => {
                        let fun = self.parse_function()?;
                        root_nodes.push(RootNode::Function(fun));
                    }
                    "record" => {
                        let record = self.parse_record()?;
                        root_nodes.push(RootNode::Record(record));
                    }
                    else_ => {
                        return Err(AstError::new(token, "unexpected root token"));
                    }
                },
                TokenKind::Comment => {
                    let value = self.c.get_token_value(&token).to_string();
                    self.skip_token();
                    root_nodes.push(RootNode::Comment(value));
                }
                _ => {
                    return Err(AstError::new(token, "unexpected root token"));
                }
            }
        }

        self.root_nodes = root_nodes;
        Ok(())
    }

    fn parse_type_info(&mut self) -> AstResult<TypeInfo> {
        let mut is_array = false;
        let mut type_str: String;
        let next_token = self.next_token();
        let kind = match self.c.get_token_value(&next_token) {
            "#" => {
                self.skip_expected_token("[")?;
                let key = self.next_token().get_string_value(self.c);
                self.skip_expected_token(",")?;
                let value = self.next_token().get_string_value(self.c);
                self.skip_expected_token("]")?;
                TypeInfoKind::HashMap(key, value)
            }
            "[" => {
                let value = self.next_token().get_string_value(self.c);
                self.skip_expected_token("]")?;
                TypeInfoKind::Array(value)
            }
            _ => {
                if ![TokenKind::Identifier, TokenKind::Keyword].contains(&next_token.kind) {
                    return Err(AstError::new(next_token, "unexpected token when parsing function return type"));
                }
                TypeInfoKind::Scalar(next_token.get_string_value(self.c))
            }
        };

        Ok(TypeInfo::new(kind))
    }

    fn parse_record(&mut self) -> AstResult<Record> {
        self.skip_expected_token("record")?;

        let record_name_token = self.need_next_identifier_token()?.clone();
        let record_name = record_name_token.get_string_value(self.c);
        self.skip_expected_token("\n")?;

        let mut fields = IndexMap::new();
        loop {
            self.skip_line_end_tokens();
            if self.try_skip_non_string_token("end") {
                break;
            }

            match self.peek_next_token().kind {
                TokenKind::Comment => {
                    self.skip_token();
                }
                _ => {
                    let field_name = self.need_next_identifier_token()?.get_string_value(self.c);
                    self.skip_expected_token(":")?;
                    let type_info = self.parse_type_info()?;
                    fields.insert(field_name, type_info);
                }
            }
        }

        let record_node = Record { name: record_name.clone(), fields };

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

    fn parse_function(&mut self) -> AstResult<Function> {
        fn try_remove_generic(s: impl AsRef<str>) -> Option<String> {
            if s.as_ref().starts_with('$') {
                let b = &s.as_ref()[1..];
                Some(b.to_string())
            } else {
                None
            }
        }

        fn update_generics_info(type_info: &mut TypeInfo, generics: &mut IndexSet<String>) {
            match type_info.kind {
                TypeInfoKind::Scalar(ref mut type_info_str) => {
                    if let Some(s) = try_remove_generic(&type_info_str) {
                        *type_info_str = s.clone();
                        generics.insert(s);
                    }
                }
                TypeInfoKind::Array(ref mut type_info_str) => {
                    if let Some(s) = try_remove_generic(&type_info_str) {
                        *type_info_str = s.clone();
                        generics.insert(s);
                    }
                }
                TypeInfoKind::HashMap(ref mut key_type_info_str, ref mut value_type_info_str) => {
                    if let Some(s) = try_remove_generic(&key_type_info_str) {
                        *key_type_info_str = s.clone();
                        generics.insert(s);
                    }
                    if let Some(s) = try_remove_generic(&value_type_info_str) {
                        *value_type_info_str = s.clone();
                        generics.insert(s);
                    }
                }
            }
        }

        self.skip_expected_token("function")?;

        let fn_name_token = self.next_token().clone();
        if fn_name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(fn_name_token, "identifier expected"));
        }

        let fn_name = fn_name_token.get_string_value(self.c);
        //let fn_generic_params = self.parse_function_declaration_generic_params()?;
        let mut fn_params = self.parse_function_declaration_params()?;
        let mut fn_generic_params = IndexSet::new();
        for (_, type_info) in &mut fn_params {
            update_generics_info(type_info, &mut fn_generic_params);
        }
        let mut fn_return_type = self.parse_function_return_type()?;
        if let Some(ref mut type_info) = fn_return_type {
            update_generics_info(type_info, &mut fn_generic_params);
        }

        let fn_declaration = FunctionDeclaration {
            token: fn_name_token.clone(),
            name: fn_name,
            generic_params: fn_generic_params,
            params: fn_params,
            return_type: fn_return_type,
        };

        if self.try_skip_non_string_token("external") {
            self.skip_line_end_tokens();
            return Ok(Function {
                is_external: true,
                declaration: fn_declaration,
                vars: IndexMap::new(),
                body: Block { statements: vec![] },
            });
        }

        let fn_body = self.parse_block()?;
        return Ok(Function {
            is_external: false,
            declaration: fn_declaration,
            vars: IndexMap::new(),
            body: fn_body,
        });
    }

    fn parse_function_declaration_generic_params(&mut self) -> AstResult<Vec<String>> {
        let mut params = vec![];

        if !self.try_skip_non_string_token("<") {
            return Ok(params);
        }

        loop {
            let generic_param_name = self.next_token().get_string_value(self.c);
            params.push(generic_param_name);
            if self.try_skip_non_string_token(">") {
                break;
            }
            self.skip_expected_token(",");
        }

        Ok(params)
    }

    fn parse_function_declaration_params(&mut self) -> AstResult<IndexMap<String, TypeInfo>> {
        if !self.try_skip_non_string_token("(") {
            return Ok(IndexMap::new());
        }

        let mut params = IndexMap::new();
        loop {
            if self.try_skip_non_string_token(")") {
                break;
            }

            let (name, type_info) = self.parse_function_declaration_param_until(&[",", ")"])?;
            params.insert(name, type_info);

            self.try_skip_non_string_token(",");
        }

        Ok(params)
    }

    fn parse_function_declaration_param_until(&mut self, terminators: &[&str]) -> AstResult<(String, TypeInfo)> {
        let name_token = self.next_token().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(name_token, "identifier expected"));
        }
        let var_name = name_token.get_string_value(self.c);
        self.skip_expected_token(":")?;
        let type_info = self.parse_type_info()?;
        Ok((var_name, type_info))
    }

    fn parse_function_return_type(&mut self) -> AstResult<Option<TypeInfo>> {
        let next_possibles = ["external", "\n"];
        let value = self.c.get_token_value(&self.peek_next_token());
        if next_possibles.contains(&value) {
            return Ok(None);
        }
        let return_type = self.parse_type_info()?;
        Ok(Some(return_type))
    }

    fn parse_function_call(&mut self) -> AstResult<FunctionCall> {
        let token = self.next_token().clone();
        let name = token.get_string_value(self.c);
        let group = self.parse_group()?;
        if let ExpressionKind::Group(expressions) = *group.kind {
            Ok(FunctionCall { name, params: expressions })
        } else {
            panic!();
        }
    }

    fn parse_block(&mut self) -> AstResult<Block> {
        let block = self.parse_block_until(&["end"])?;
        self.skip_token();
        return Ok(block);
    }

    fn parse_block_until(&mut self, terminators: &[&str]) -> AstResult<Block> {
        let mut statements = vec![];
        loop {
            self.skip_line_end_tokens();
            if self.check_next_non_string_token_values(terminators) {
                break;
            }
            let statement = self.parse_statement()?;
            statements.push(statement);
        }

        Ok(Block { statements })
    }

    fn parse_statement(&mut self) -> AstResult<Statement> {
        let token = self.peek_next_token().clone();
        match token.kind {
            TokenKind::Identifier => {
                match self.c.get_token_value(&self.peek_next_next_token()) {
                    // "(" => {
                    //     let f = self.parse_function_call()?;
                    //     return Ok(Statement::FunctionCall(f));
                    // }
                    "=" => {
                        let statement = self.parse_variable_assignment_statement()?;
                        return Ok(Statement::VariableAssignment(statement));
                    }
                    ":" => {
                        let statement = self.parse_variable_declaration_statement()?;
                        return Ok(Statement::VariableDeclaration(statement));
                    }
                    ">>" => {
                        let statement = self.parse_iteration_statement()?;
                        return Ok(Statement::Iteration(statement));
                    }
                    _ => {}
                }

                let left_side_expression = self.parse_expression_until(&["=", "\n"])?;
                if self.try_skip_non_string_token("=") {
                    let right_side_expression = self.parse_expression_until(&["\n"])?;
                    return Ok(Statement::Assignment(Assignment {
                        token: token.clone(),
                        lvalue: left_side_expression,
                        rvalue: right_side_expression,
                    }));
                } else {
                    return Ok(Statement::Expression(left_side_expression));
                }
            }

            TokenKind::Keyword => match self.c.get_token_value(&token) {
                "if" => {
                    let f = self.parse_if_else_statement()?;
                    return Ok(Statement::If(f));
                }
                "loop" => {
                    let f = self.parse_loop_statement()?;
                    return Ok(Statement::Loop(f));
                }
                "for" => {
                    let statement = self.parse_for_statement()?;
                    return Ok(Statement::For(statement));
                }
                "break" => {
                    self.skip_expected_token("break")?;
                    return Ok(Statement::Break());
                }
                "continue" => {
                    self.skip_expected_token("continue")?;
                    return Ok(Statement::Continue());
                }
                "return" => {
                    self.skip_expected_token("return")?;
                    if self.try_skip_non_string_token("\n") {
                        return Ok(Statement::Return(None));
                    } else {
                        let expression = self.parse_expression_until(&["\n"])?;
                        return Ok(Statement::Return(Some(expression)));
                    }
                }
                _ => return Err(AstError::new(token, "unexpected keyword")),
            },

            TokenKind::Comment => {
                let value = token.get_string_value(self.c);
                self.skip_token();
                Ok(Statement::Comment(value))
            }

            _ => return Err(AstError::new(token, "unexpected token when parsing statement")),
        }
    }

    fn parse_if_else_statement(&mut self) -> AstResult<IfElseStatement> {
        self.skip_expected_token("if")?;

        let mut if_blocks: Vec<(Expression, Block)> = Vec::new();
        let mut else_block = None;

        let condition = self.parse_expression_until(&["then", "\n"])?;

        if self.try_skip_non_string_token("then") {
            let statement = self.parse_statement()?;
            let block = Block { statements: vec![statement] };
            if_blocks.push((condition, block));
            return Ok(IfElseStatement { if_blocks, else_block: None });
        }

        let block = self.parse_block_until(&["else", "end"])?;
        if_blocks.push((condition, block));

        loop {
            self.skip_line_end_tokens();
            if self.try_skip_non_string_token("end") {
                self.skip_token();
                break;
            } else if self.try_skip_non_string_token("else") {
                if self.try_skip_non_string_token("if") {
                    let condition = self.parse_expression_until(&["\n"])?;
                    let block = self.parse_block_until(&["else", "end"])?;
                    if_blocks.push((condition, block));
                } else {
                    else_block = Some(self.parse_block()?);
                    break;
                }
            } else {
                return Err(AstError::new(self.peek_next_token(), "unexpected token in if/else statement"));
            }
        }

        Ok(IfElseStatement { if_blocks, else_block })
    }

    fn parse_loop_statement(&mut self) -> AstResult<LoopStatement> {
        self.skip_expected_token("loop")?;

        let block = self.parse_block()?;
        Ok(LoopStatement { block })
    }

    fn parse_iteration_statement(&mut self) -> AstResult<IterationStatement> {
        let name_token = self.next_token().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(name_token, "identifier expected"));
        }
        let iterable_name = name_token.get_string_value(self.c);

        self.skip_expected_token(">>")?;

        let block = if self.c.get_token_value(&self.peek_next_token()) != "{" {
            let statement = self.parse_statement()?;
            Block { statements: vec![statement] }
        } else {
            self.parse_block()?
        };

        Ok(IterationStatement {
            token: name_token.clone(),
            iterable_name,
            block,
        })
    }

    fn parse_for_statement(&mut self) -> AstResult<ForStatement> {
        self.skip_expected_token("for")?;
        let iterable_expression = self.parse_expression_until(&["{", "do"])?;

        let block = if self.try_skip_non_string_token("do") {
            let statement = self.parse_statement()?;
            Block { statements: vec![statement] }
        } else {
            self.parse_block()?
        };

        Ok(ForStatement { iterable_expression, block })
    }

    fn parse_variable_assignment_statement(&mut self) -> AstResult<VariableAssignment> {
        let name_token = self.need_next_identifier_token()?.clone();
        let var_name = name_token.get_string_value(self.c);
        self.skip_expected_token("=")?;

        let expression = self.parse_expression_until(&["\n"])?;

        Ok(VariableAssignment {
            token: name_token.clone(),
            name: var_name.clone(),
            rvalue_expression: expression,
        })
    }

    fn parse_variable_declaration_statement(&mut self) -> AstResult<(Token, String, TypeInfo)> {
        let name_token = self.next_token().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(name_token, "identifier expected"));
        }
        let var_name = name_token.get_string_value(self.c);
        self.skip_expected_token(":")?;
        let type_info = self.parse_type_info()?;
        Ok((name_token.clone(), var_name, type_info))
    }

    fn parse_group(&mut self) -> AstResult<Expression> {
        let group_first_token = self.peek_next_token().clone();
        self.skip_expected_token("(")?;
        let mut expressions = vec![];
        loop {
            if self.peek_next_token().kind != TokenKind::StringLiteral && self.try_skip_non_string_token(")") {
                break;
            }

            let expression = self.parse_expression_until(&[",", ")"])?;
            expressions.push(expression);

            self.try_skip_non_string_token(",");
        }

        Ok(Expression::new(group_first_token, ExpressionKind::Group(expressions)))
    }

    fn parse_array_brackets(&mut self) -> AstResult<Expression> {
        let token = self.peek_next_token().clone();
        self.skip_expected_token("[")?;
        let expression = self.parse_expression_until(&["]"])?;
        self.skip_expected_token("]")?;

        Ok(Expression::new(token, ExpressionKind::ArrayBrackets(expression)))
    }

    fn parse_hashmap_initializer(&mut self) -> AstResult<Expression> {
        let token = self.peek_next_token().clone();
        let type_info = self.parse_type_info()?;
        match type_info.kind {
            TypeInfoKind::HashMap(k, v) => {
                return Ok(Expression::new(token, ExpressionKind::HashMapInitializer(k, v)));
            }
            _ => return Err(AstError::new(token, format!("error parsing hash map type"))),
        }
        let token = self.peek_next_token().clone();
        self.skip_expected_token("[")?;
        let expression = self.parse_expression_until(&["]"])?;
        self.skip_expected_token("]")?;

        Ok(Expression::new(token, ExpressionKind::ArrayBrackets(expression)))
    }

    fn parse_object_initializer(&mut self) -> AstResult<Expression> {
        let token = self.peek_next_token().clone();
        self.skip_expected_token("{")?;
        self.skip_expected_token("}")?;

        Ok(Expression::new(token, ExpressionKind::ObjectLiteral))
    }

    /// stage 1: parse expression tokens into list (get_expressions)
    /// stage 2: convert list into intermediate tree by operation precedence (expressions_to_tree)
    /// stage 3: convert intermediate tree into expression (tree_to_expression)
    fn parse_expression_until(&mut self, terminators: &[&str]) -> AstResult<Expression> {
        let mut expressions = self.get_expressions(terminators)?;
        if expressions.len() == 0 {
            return Err(AstError::new(self.peek_next_token(), "empty expression"));
        }
        let (nodes, root_id) = self.expressions_to_tree(&expressions)?;
        let expr = self.node_to_expression(&nodes, root_id)?;
        Ok(expr)
    }

    fn get_expressions(&mut self, terminators: &[&str]) -> AstResult<Vec<Expression>> {
        let mut expressions = vec![];
        loop {
            let token = self.peek_next_token();

            if token.kind != TokenKind::StringLiteral && terminators.contains(&self.c.get_token_value(&token)) {
                break;
            }

            if token.kind == TokenKind::SpecialSymbol {
                match self.c.get_token_value(&token) {
                    "(" => {
                        let expression = self.parse_group()?;
                        expressions.push(expression);
                        continue;
                    }
                    "[" => {
                        let expression = self.parse_array_brackets()?;
                        expressions.push(expression);
                        continue;
                    }
                    "#" => {
                        let expression = self.parse_hashmap_initializer()?;
                        expressions.push(expression);
                        continue;
                    }
                    "{" => {
                        let expression = self.parse_object_initializer()?;
                        expressions.push(expression);
                        continue;
                    }
                    _ => {}
                }
            }

            let token = self.next_token();
            let value = token.get_string_value(self.c);
            match token.kind {
                TokenKind::LineEnd => {
                    break;
                }
                TokenKind::Identifier => {
                    let expression = Expression::new(token, ExpressionKind::Identifier(value));
                    expressions.push(expression);
                }
                TokenKind::Keyword => {
                    let expression = Expression::new(token, ExpressionKind::Identifier(value));
                    expressions.push(expression);
                }
                TokenKind::SpecialSymbol => {
                    let expression = Expression::new(token, ExpressionKind::Operator(value));
                    expressions.push(expression);
                }
                TokenKind::IntegerLiteral => {
                    let expression = Expression::new(token, ExpressionKind::IntegerLiteral(value));
                    expressions.push(expression);
                }
                TokenKind::RealLiteral => {
                    let expression = Expression::new(token, ExpressionKind::RealLiteral(value));
                    expressions.push(expression);
                }
                TokenKind::BooleanLiteral => {
                    let expression = Expression::new(token, ExpressionKind::BooleanLiteral(value));
                    expressions.push(expression);
                }
                TokenKind::StringLiteral => {
                    let expression = Expression::new(token, ExpressionKind::StringLiteral(value));
                    expressions.push(expression);
                }
                TokenKind::Comment => {
                    break;
                }
                _ => {
                    return Err(AstError::new(token, "unexpected token"));
                }
            }
        }

        Ok(expressions)
    }

    fn get_suitable_target(&self, arena: &Arena<Node>, prev_id: Id<Node>, op: &str) -> AstResult<(Option<Id<Node>>)> {
        let mut cur_id = prev_id;
        let mut cur = &arena[cur_id];

        if cur.expression.is_op() {
            return Ok(None);
        }

        let op_priority = *self.priorities.get(&(op.to_string(), OpPriorityKind::Binary)).unwrap();

        let mut target_id = None;
        while let Some(parent_id) = cur.parent_id {
            let mut parent = &arena[parent_id];
            match &*parent.expression.kind {
                ExpressionKind::Operator(parent_op) => {
                    let parent_op_kind = if parent.is_unary() { OpPriorityKind::Unary } else { OpPriorityKind::Binary };
                    let parent_op_priority = *self.priorities.get(&(parent_op.to_string(), parent_op_kind)).unwrap();
                    if op_priority <= parent_op_priority {
                        target_id = Some(parent_id)
                    } else {
                        break;
                    }
                }
                _ => return Err(AstError::new(parent.expression.token, "unexpected token")),
            }
            cur = parent;
            cur_id = parent_id;
        }

        if let Some(id) = target_id {
            Ok(Some(id))
        } else {
            Ok(Some(prev_id))
        }
    }

    fn expressions_to_tree(&self, expressions: &Vec<Expression>) -> AstResult<(Arena<Node>, Id<Node>)> {
        let mut nodes = Arena::<Node>::new();
        let mut expressions_iter = expressions.iter();
        let expr = expressions_iter.next().unwrap();
        let mut prev_id = nodes.alloc(Node::new(expr));
        while let Some(expr) = expressions_iter.next() {
            let root_id = get_root_id(&nodes, prev_id);
            //println!("{:?}", expr.kind);
            let mut node_id = nodes.alloc(Node::new(expr));
            match &*expr.kind {
                ExpressionKind::Operator(op_str) => {
                    if let Some(target_id) = self.get_suitable_target(&nodes, prev_id, op_str)? {
                        let parent_id = nodes[target_id].parent_id;
                        if let Some(parent_id) = parent_id {
                            nodes[parent_id].right_id = Some(node_id);
                        }
                        nodes[node_id].parent_id = parent_id;
                        nodes[node_id].left_id = Some(target_id);
                        nodes[target_id].parent_id = Some(node_id);
                        prev_id = node_id;
                        continue;
                    }
                }

                ExpressionKind::Group(group_expressions) => {
                    //
                    if let ExpressionKind::Identifier(identifier_name) = &*nodes[prev_id].expression.kind {
                        let kind = ExpressionKind::FunctionCall(FunctionCall {
                            name: identifier_name.clone(),
                            params: group_expressions.clone(),
                        });
                        let expression = Expression::new(nodes[prev_id].expression.token, kind);
                        nodes[prev_id].expression = expression;
                        continue;
                    }
                }

                ExpressionKind::ObjectLiteral => {
                    //
                    match &*nodes[prev_id].expression.kind {
                        ExpressionKind::Identifier(identifier_name) => {
                            let kind = ExpressionKind::ObjectInitializer(identifier_name.clone());
                            let expression = Expression::new(nodes[prev_id].expression.token, kind);
                            nodes[prev_id].expression = expression;
                            break;
                        }
                        else_ => return Err(AstError::new(nodes[node_id].expression.token, "unexpected token")),
                    }
                }

                ExpressionKind::ArrayBrackets(array_indexed_access_expression) => {
                    if let Some(target_id) = self.get_suitable_target(&nodes, prev_id, "[]")? {
                        let parent_id = nodes[target_id].parent_id;
                        if let Some(parent_id) = parent_id {
                            nodes[parent_id].right_id = Some(node_id);
                        }
                        nodes[node_id].parent_id = parent_id;
                        nodes[node_id].left_id = Some(target_id);
                        nodes[target_id].parent_id = Some(node_id);
                        prev_id = node_id;
                        continue;
                    }
                }

                _ => {
                    //todo: fix invalid sequences like 'identifier identifier' etc.
                    //return Err(AstError::new(expr.token, "unexpected token"))
                }
            }

            nodes[node_id].parent_id = Some(prev_id);
            nodes[prev_id].right_id = Some(node_id);
            prev_id = node_id;
        }
        let root_id = get_root_id(&nodes, prev_id);
        Ok((nodes, root_id))
    }

    fn node_to_expression(&self, nodes: &Arena<Node>, id: Id<Node>) -> AstResult<Expression> {
        match &*nodes[id].expression.kind {
            ExpressionKind::Operator(op) => {
                if let Some(left_id) = nodes[id].left_id {
                    let left = self.node_to_expression(nodes, left_id)?;
                    let right = self.node_to_expression(nodes, nodes[id].right_id.unwrap())?;
                    let kind = ExpressionKind::BinaryOperation {
                        operator: op.clone(),
                        left: left,
                        right: right,
                    };

                    let expression = Expression::new(nodes[id].expression.token, kind);
                    return Ok(expression);
                } else {
                    let expr = self.node_to_expression(nodes, nodes[id].right_id.unwrap())?.clone();
                    let kind = ExpressionKind::UnaryOperation { operator: op.clone(), expr };

                    let expression = Expression::new(nodes[id].expression.token, kind);
                    return Ok(expression);
                }
            }

            ExpressionKind::ArrayBrackets(access_expression) => {
                if let Some(left_id) = nodes[id].left_id {
                    let array_expression = self.node_to_expression(&nodes, left_id)?;
                    let kind = ExpressionKind::ArrayItemAccess {
                        array_expression,
                        access_expression: access_expression.clone(),
                    };
                    let expression = Expression::new(nodes[id].expression.token, kind);
                    return Ok(expression);
                } else {
                    match &*access_expression.kind {
                        ExpressionKind::Identifier(identifier) => {
                            let kind = ExpressionKind::ArrayInitializer(identifier.clone());
                            let expression = Expression::new(nodes[id].expression.token, kind);
                            return Ok(expression);
                        }
                        _ => return Err(AstError::new(access_expression.token, "expected identifier in array initializer")),
                    }
                }
            }

            else_ => {
                //assert!(false, "can't detect type");
                return Ok(nodes[id].expression.clone());
            }
        }
    }
}

#[derive(Debug)]
pub struct Node {
    expression: Expression,
    parent_id: Option<Id<Node>>,
    left_id: Option<Id<Node>>,
    right_id: Option<Id<Node>>,
}

impl Node {
    fn new(expression: &Expression) -> Node {
        Self {
            expression: expression.clone(),
            parent_id: None,
            left_id: None,
            right_id: None,
        }
    }

    fn is_unary(&self) -> bool {
        self.left_id.is_none()
    }
}

fn get_root_id(nodes: &Arena<Node>, id: Id<Node>) -> Id<Node> {
    let mut root_id = id;
    while let Some(parent_id) = nodes[root_id].parent_id {
        root_id = parent_id;
    }
    root_id
}

/// utils 

fn create_priorities() -> HashMap<(String, OpPriorityKind), usize> {
    fn add_op_priority(priorities: &mut HashMap<(String, OpPriorityKind), usize>, op: impl Into<String>, op_kind: OpPriorityKind, value: usize) {
        priorities.insert((op.into(), op_kind), value);
    }

    let mut p = HashMap::new();
    add_op_priority(&mut p, ".", OpPriorityKind::Binary, 200);
    add_op_priority(&mut p, "()", OpPriorityKind::Binary, 200);
    add_op_priority(&mut p, "[]", OpPriorityKind::Binary, 200);

    add_op_priority(&mut p, "not", OpPriorityKind::Unary, 110);
    add_op_priority(&mut p, "-", OpPriorityKind::Unary, 110);

    add_op_priority(&mut p, "*", OpPriorityKind::Binary, 100);
    add_op_priority(&mut p, "/", OpPriorityKind::Binary, 100);
    add_op_priority(&mut p, "div", OpPriorityKind::Binary, 100);
    add_op_priority(&mut p, "mod", OpPriorityKind::Binary, 100);

    add_op_priority(&mut p, "+", OpPriorityKind::Binary, 90);
    add_op_priority(&mut p, "-", OpPriorityKind::Binary, 90);

    add_op_priority(&mut p, "..", OpPriorityKind::Binary, 80);

    add_op_priority(&mut p, "bit_shift_left", OpPriorityKind::Binary, 60);
    add_op_priority(&mut p, "bit_shift_right", OpPriorityKind::Binary, 60);

    add_op_priority(&mut p, "<", OpPriorityKind::Binary, 50);
    add_op_priority(&mut p, "<=", OpPriorityKind::Binary, 50);
    add_op_priority(&mut p, ">", OpPriorityKind::Binary, 50);
    add_op_priority(&mut p, ">=", OpPriorityKind::Binary, 50);

    add_op_priority(&mut p, "=", OpPriorityKind::Binary, 40);
    add_op_priority(&mut p, "<>", OpPriorityKind::Binary, 40);

    add_op_priority(&mut p, "bit_xor", OpPriorityKind::Binary, 35);

    add_op_priority(&mut p, "and", OpPriorityKind::Binary, 30);

    add_op_priority(&mut p, "or", OpPriorityKind::Binary, 20);

    return p;
}