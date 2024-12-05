#![allow(warnings)]
use std::collections::hash_map::Entry;
use std::collections::HashMap;
use std::fmt::Write;
use std::path::Iter;
use std::{fmt, vec};

use indexmap::IndexMap;

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
    pub is_external: bool,
    pub declaration: FnDeclaration,
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
pub struct IfStatement {
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
pub enum ExpressionKind {
    FunctionCall(FunctionCall),
    IntegerLiteral(String),
    RealLiteral(String),
    StringLiteral(String),
    Identifier(String),
    BinaryOperation {
        operation: String,
        left: Expression,
        right: Expression,
    },
    Group(Vec<Expression>),
    Object(String),
    ArrayItemAccess {
        array_expression: Expression,
        access_expression: Expression,
    },

    // todo low: tmp operators, used only on parsing stage, not presented in final tree, needs refactoring
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
    pub fn new(token: &Token, kind: ExpressionKind) -> Expression {
        Expression {
            token: token.clone(),
            kind: Box::new(kind),
            type_info: None,
        }
    }
}

#[derive(Debug, Clone)]
pub enum Statement {
    If(IfStatement),
    Loop(LoopStatement),
    Iteration(IterationStatement),
    Break(),
    Return(Expression),
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

        if self.try_skip("external") {
            self.skip_line_end_tokens();
            return Ok(Function {
                is_external: true,
                declaration: fn_declaration,
                vars: IndexMap::new(),
                body: Block { statements: vec![] },
            });
        }
        
        let fn_body = self.parse_function_body()?;
        return Ok(Function {
            is_external: false,
            declaration: fn_declaration,
            vars: IndexMap::new(),
            body: fn_body,
        });
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
        let next_possibles = ["external", "\n"];

        if next_possibles.contains(&self.peek_next().value.as_str()) {
            return Ok(None);
        }

        let return_type = self.parse_type_info_until(&next_possibles)?;

        Ok(Some(return_type))
    }

    fn parse_function_body(&mut self) -> AstResult<Block> {
        self.skip_line_end_tokens();
        //self.skip_expected("begin")?;
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
        let group = self.parse_group()?;
        if let ExpressionKind::Group(expressions) = *group.kind {
            Ok(FunctionCall {
                name,
                params: expressions,
            })
        } else {
            panic!();
        }
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

    fn parse_statement(&mut self) -> AstResult<Statement> {
        let token = self.peek_next().clone();
        match token.kind {
            TokenKind::Identifier => {
                match self.peek_next_next().value.as_str() {
                    "(" => {
                        let f = self.parse_function_call()?;
                        return Ok(Statement::FunctionCall(f));
                    }

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
                self.skip_expected("=");
                let right_side_expression = self.parse_expression_until(&["\n"])?;
                return Ok(Statement::Assignment(Assignment {
                    token: token.clone(),
                    lvalue: left_side_expression,
                    rvalue: right_side_expression,
                }));
            }

            TokenKind::Keyword => match token.value.as_str() {
                "if" => {
                    let f = self.parse_if_statement()?;
                    return Ok(Statement::If(f));
                }
                "loop" => {
                    let f = self.parse_loop_statement()?;
                    return Ok(Statement::Loop(f));
                }
                "break" => {
                    self.skip_expected("break")?;
                    return Ok(Statement::Break());
                }
                "ret" => {
                    self.skip_expected("ret")?;
                    let expression = self.parse_expression_until(&["\n"])?;
                    return Ok(Statement::Return(expression));
                }
                panic_ => {
                    panic!();
                }
            },

            TokenKind::Comment => {
                let value = token.value.clone();
                self.skip();
                Ok(Statement::Comment(value))
            }

            _ => {
                println!("{token:?}");
                panic!();
            }
        }
    }

    fn parse_if_statement(&mut self) -> AstResult<IfStatement> {
        self.skip_expected("if")?;

        let mut if_blocks: Vec<(Expression, Block)> = Vec::new();
        let mut else_block = None;

        let condition = self.parse_expression_until(&["do", "{"])?;

        if self.try_skip("do") {
            let statement = self.parse_statement()?;
            let block = Block { statements: vec![statement] };
            if_blocks.push((condition, block));
            return Ok(IfStatement { if_blocks, else_block: None })
        }

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

    fn parse_iteration_statement(&mut self) -> AstResult<IterationStatement> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstError::new(&name_token, "identifier expected"));
        }
        let iterable_name = name_token.value.clone();

        self.skip_expected(">>")?;

        let block = if self.peek_next().value != "{" {
            let statement = self.parse_statement()?;
            Block {
                statements: vec![statement],
            }
        } else {
            self.parse_block()?
        };

        Ok(IterationStatement {
            token: name_token.clone(),
            iterable_name,
            block,
        })
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

    fn parse_group(&mut self) -> AstResult<Expression> {
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

        Ok(Expression::new(&token, ExpressionKind::Group(expressions)))
    }

    fn parse_array_brackets(&mut self) -> AstResult<Expression> {
        let token = self.peek_next().clone();
        self.skip_expected("[")?;
        let expression = self.parse_expression_until(&[",", "]"])?;
        self.skip_expected("]")?;

        Ok(Expression::new(&token, ExpressionKind::ArrayBrackets(expression)))
    }

    fn parse_object_initializer(&mut self) -> AstResult<Expression> {
        let token = self.peek_next().clone();
        self.skip_expected("{")?;
        self.skip_expected("}")?;

        Ok(Expression::new(&token, ExpressionKind::ObjectLiteral))
    }

    fn parse_expression_until(&mut self, terminators: &[&str]) -> AstResult<Expression> {
        let mut expressions = vec![];
        loop {
            let t = self.peek_next();

            if terminators.contains(&t.value.as_str()) {
                break;
            }

            if t.kind == TokenKind::SpecialSymbol {
                match t.value.as_str() {
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
                    "{" => {
                        let expression = self.parse_object_initializer()?;
                        expressions.push(expression);
                        continue;
                    }
                    _ => {}
                }
            }

            let token = self.next();
            let value = token.value.clone();
            match token.kind {
                TokenKind::LineEnd => {
                    break;
                }
                TokenKind::Identifier => {
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
                TokenKind::FloatLiteral => {
                    let expression = Expression::new(token, ExpressionKind::RealLiteral(value));
                    expressions.push(expression);
                }
                TokenKind::String => {
                    let expression = Expression::new(token, ExpressionKind::StringLiteral(value));
                    expressions.push(expression);
                }
                _ => (),
            }
        }

        let mut root = TreeNode {
            expression: expressions[0].clone(),
            childs: vec![],
        };

        let mut index = 1;
        loop {
            if index < expressions.len() {
                root = self.expressions_to_tree_node(&expressions, &mut index, root, 0)?;
                //println!("{:?}", root);
            } else {
                break;
            }
        }

        let node = self.tree_node_to_expression(&root)?;
        Ok(node)
    }

    fn expressions_to_tree_node(
        &mut self,
        expressions: &Vec<Expression>,
        next_expression_index: &mut usize,
        tree_node: TreeNode,
        priority: usize,
    ) -> AstResult<TreeNode> {
        let mut tree_node = tree_node;
        let mut priority = priority;
        loop {
            if *next_expression_index >= expressions.len() {
                break;
            }

            let next_expression = &expressions[*next_expression_index];
            //println!("{next_expression:?}");

            match &*next_expression.kind {
                ExpressionKind::Operator(operator) => {
                    let next_priority = *self.tokenizer.priorities.get(operator).unwrap();

                    if next_priority <= priority {
                        break;
                    }

                    let next_next_ast_node = &expressions[*next_expression_index + 1];
                    let mut next_tree_node = TreeNode {
                        expression: next_next_ast_node.clone(),
                        childs: vec![],
                    };

                    *next_expression_index += 2;
                    next_tree_node = self.expressions_to_tree_node(
                        expressions,
                        next_expression_index,
                        next_tree_node,
                        next_priority,
                    )?;

                    let mut childs = vec![];
                    childs.push(tree_node);
                    childs.push(next_tree_node);
                    tree_node = TreeNode {
                        expression: next_expression.clone(),
                        childs,
                    }
                }

                ExpressionKind::Group(group_expressions) => {
                    //
                    match &*tree_node.expression.kind {
                        ExpressionKind::Identifier(identifier_name) => {
                            let kind = ExpressionKind::FunctionCall(FunctionCall {
                                name: identifier_name.clone(),
                                params: group_expressions.clone(),
                            });

                            let expression = Expression::new(&tree_node.expression.token, kind);

                            tree_node.expression = expression;
                            *next_expression_index += 1;
                            break;
                        }

                        else_ => {
                            println!("{:?}", tree_node.expression);
                            panic!()
                        }
                    }
                }

                ExpressionKind::ArrayBrackets(array_indexed_access_expression) => {
                    let next_priority = *self.tokenizer.priorities.get("[]").unwrap();

                    if next_priority <= priority {
                        break;
                    }

                    tree_node = TreeNode {
                        expression: next_expression.clone(),
                        childs: vec![tree_node]
                    };
                    *next_expression_index += 1;
                    break;
                }

                ExpressionKind::ObjectLiteral => {
                    //
                    match &*tree_node.expression.kind {
                        ExpressionKind::Identifier(identifier_name) => {
                            let kind = ExpressionKind::Object(identifier_name.clone());
                            let expression = Expression::new(&tree_node.expression.token, kind);
                            tree_node.expression = expression;
                            *next_expression_index += 1;
                            break;
                        }

                        else_ => {
                            println!("{:?}", tree_node.expression);
                            panic!()
                        }
                    }
                }

                _ => {
                    return Err(AstError::new(&next_expression.token, "unexpected token"));
                }
            }
        }

        Ok(tree_node)
    }

    fn tree_node_to_expression(&self, tree_node: &TreeNode) -> AstResult<Expression> {
        match &*tree_node.expression.kind {
            ExpressionKind::Operator(op) => {
                let left = self.tree_node_to_expression(&tree_node.childs[0])?;
                let right = self.tree_node_to_expression(&tree_node.childs[1])?;
                // println!("{:?}", left);
                // println!("{:?}", right);
                let kind = ExpressionKind::BinaryOperation {
                    operation: op.clone(),
                    left: left,
                    right: right,
                };

                let expression = Expression::new(&tree_node.expression.token, kind);
                return Ok(expression);
            }

            ExpressionKind::ArrayBrackets(access_expression) => {
                let array_expression = self.tree_node_to_expression(&tree_node.childs[0])?;
                let kind = ExpressionKind::ArrayItemAccess { 
                    array_expression, 
                    access_expression: access_expression.clone()
                };

                let expression = Expression::new(&tree_node.expression.token, kind);
                return Ok(expression);
            }

            else_ => {
                //assert!(false, "can't detect type");
                return Ok(tree_node.expression.clone());
            }
        }
    }
}

#[derive(Debug)]
struct TreeNode {
    expression: Expression,
    childs: Vec<TreeNode>,
}
