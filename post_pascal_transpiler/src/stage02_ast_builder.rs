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

#[derive(Debug, Clone)]
pub struct ProgramRootNode {
    pub children: Vec<AstNode>,
}

#[derive(Debug, Clone)]
pub struct FunctionDeclarationParam {
    pub name: String,
    pub type_info: TypeInfo,
}

#[derive(Debug, Clone)]
pub struct FunctionNode {
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
    pub statements: Vec<AstNode>,
}

#[derive(Debug, Clone)]
pub struct FunctionParamNode {
    pub expression: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct ExpressionNode {
    //pub parts: Vec<AstNode>,
    pub root: Box<AstNode>,
    pub type_str: String,
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
pub struct BinaryOperationNode {
    pub type_str: String,
    pub operation: OperatorNode,
    pub children: Vec<AstNode>,
}

#[derive(Debug, Clone)]
pub struct IfStatementNode {
    pub if_blocks: Vec<(ExpressionNode, BlockNode)>,
    pub else_block: Option<BlockNode>,
}

#[derive(Debug, Clone)]
pub struct ForStatementNode {
    pub iterator_variable_name: String,
    pub iteratable: ExpressionNode,
    pub for_block: BlockNode,
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
pub enum AstNode {
    Program(ProgramRootNode),
    Function(FunctionNode),
    Record(RecordNode),
    Block(BlockNode),
    IfStatement(IfStatementNode),
    ForStatement(ForStatementNode),
    LoopStatement(LoopStatementNode),
    Iteration(IterationNode),
    BreakStatement(),
    ReturnStatement(ExpressionNode),
    FunctionCall(FunctionCallNode),
    VariableAssignment(VariableAssignmentNode),
    Assignment(AssignmentNode),
    VariableDeclaration(VariableDeclarationNode),
    IntegerLiteral(IntegerLiteralNode),
    RealLiteral(RealLiteralNode),
    StringLiteral(StringLiteralNode),
    Identifier(IdentifierNode),
    Operator(OperatorNode),
    Comment(CommentNode),
    BinaryOperation(BinaryOperationNode),
    Group(GroupNode),
    ArrayBrackets(ArrayBracketsNode),
    ArrayItemAccess(ArrayItemAccessNode),
}

pub struct AstBuilderError {
    pub token: Token,
    pub expected: String,
}

impl AstBuilderError {
    pub fn new(token: Token, s: impl Into<String>) -> AstBuilderError {
        AstBuilderError {
            token,
            expected: s.into(),
        }
    }
}

type AstResult<T> = Result<T, AstBuilderError>;

#[derive(Debug, Clone)]
pub struct TypeInfo {
    pub is_array: bool,
    pub type_str: String,
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
    pub root: ProgramRootNode,
    pub records: HashMap<String, RecordNode>,
    current_function_context: Option<CurrentFunctionContext>,
}

impl AstBuilder {
    pub fn new(tokenizer: &Tokenizer) -> AstBuilder {
        AstBuilder {
            ti: 0,
            tokenizer: tokenizer.clone(),
            tokens: tokenizer.tokens.clone(),
            root: ProgramRootNode { children: vec![] },
            records: HashMap::new(),
            current_function_context: None,
        }
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
            return Err(AstBuilderError::new(token.clone(), "expected identifier"));
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
                return Err(AstBuilderError::new(token.clone(), check));
            }
            self.ti += 1;
            Ok(())
        } else {
            let token = Token {
                kind: TokenKind::Eof,
                char_index: self.tokenizer.chars.len() - 1,
                value: "eof".to_string(),
            };
            Err(AstBuilderError::new(token, check))
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
        let mut root = ProgramRootNode { children: vec![] };

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
                        root.children.push(AstNode::Function(fun));
                    }
                    "rec" => {
                        let record = self.parse_record()?;
                        root.children.push(AstNode::Record(record));
                    }
                    else_ => {
                        return Err(AstBuilderError::new(token.clone(), "unexpected token"));
                    }
                },
                TokenKind::Comment => {
                    let value = token.value.clone();
                    self.skip();
                    root.children.push(AstNode::Comment(CommentNode { value }));
                }
                _ => {
                    return Err(AstBuilderError::new(token.clone(), "unexpected token"));
                }
            }
        }

        self.root = root;
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
                return Err(AstBuilderError::new(record_name_token, "duplicate record identifier"));
            }
            Entry::Vacant(vacant_entry) => {
                vacant_entry.insert(record_node.clone());
            }
        }

        Ok(record_node)
    }

    fn parse_function(&mut self) -> AstResult<FunctionNode> {
        self.skip_expected("fn")?;

        self.current_function_context = Some(CurrentFunctionContext {
            params: IndexMap::new(),
            vars: HashMap::new(),
            iterators: vec![],
        });

        let fn_name_token = self.next();
        if fn_name_token.kind != TokenKind::Identifier {
            return Err(AstBuilderError::new(fn_name_token.clone(), "identifier expected"));
        }

        let fn_name = fn_name_token.value.clone();
        let fn_params = self.parse_function_declaration_params()?;
        let fn_return_type = self.parse_function_return_type()?;
        self.ctx().params = fn_params.clone();
        let fn_body = self.parse_function_body()?;
        //let vars = mem::take(&mut self.current_function_vars);
        let fn_vars = self.ctx().vars.clone();

        Ok(FunctionNode {
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
            return Err(AstBuilderError::new(name_token, "identifier expected"));
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

        Ok(BlockNode { statements })
    }

    fn parse_function_call(&mut self) -> AstResult<FunctionCallNode> {
        let t = self.next();
        let name = t.value.clone();
        let params_group = self.parse_group()?;
        Ok(FunctionCallNode { name, params_group })
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

        Ok(BlockNode { statements })
    }

    fn parse_statement(&mut self) -> AstResult<AstNode> {
        let token = self.peek_next();
        match token.kind {
            TokenKind::Identifier => {
                match self.peek_next_next().value.as_str() {
                    "(" => {
                        let f = self.parse_function_call()?;
                        return Ok(AstNode::FunctionCall(f));
                    }

                    "=" => {
                        let f = self.parse_variable_assignment()?;
                        return Ok(AstNode::VariableAssignment(f));
                    }

                    ":" => {
                        let f = self.parse_variable_declaration()?;
                        return Ok(AstNode::VariableDeclaration(f));
                    }

                    ">>" => {
                        let f = self.parse_iteration()?;
                        return Ok(AstNode::Iteration(f));
                    }

                    _ => {}
                }

                let left_side_expression = self.parse_expression_until(&["=", "\n"])?;
                self.skip_expected("=");
                let right_side_expression = self.parse_expression_until(&["\n"])?;
                return Ok(AstNode::Assignment(AssignmentNode {
                    lvalue: left_side_expression,
                    rvalue: right_side_expression,
                }));
            }

            TokenKind::Keyword => match token.value.as_str() {
                "if" => {
                    let f = self.parse_if_statement()?;
                    return Ok(AstNode::IfStatement(f));
                }

                "for" => {
                    let f = self.parse_for_statement()?;
                    return Ok(AstNode::ForStatement(f));
                }

                "loop" => {
                    let f = self.parse_loop_statement()?;
                    return Ok(AstNode::LoopStatement(f));
                }

                "break" => {
                    self.skip_expected("break")?;
                    return Ok(AstNode::BreakStatement());
                }

                "ret" => {
                    self.skip_expected("ret")?;
                    let expression_node = self.parse_expression_until(&["\n"])?;
                    return Ok(AstNode::ReturnStatement(expression_node));
                }

                panic_ => {
                    panic!();
                }
            },

            TokenKind::Comment => {
                let value = token.value.clone();
                self.skip();
                Ok(AstNode::Comment(CommentNode { value }))
            }

            TokenKind::SpecialSymbol => {
                let value = token.value.clone();
                self.skip();
                Ok(AstNode::Comment(CommentNode { value }))
            }

            _ => todo!(),
        }
    }

    fn parse_if_statement(&mut self) -> AstResult<IfStatementNode> {
        self.skip_expected("if")?;

        let mut if_blocks: Vec<(ExpressionNode, BlockNode)> = Vec::new();
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

        Ok(IfStatementNode { if_blocks, else_block })
    }

    fn parse_loop_statement(&mut self) -> AstResult<LoopStatementNode> {
        self.skip_expected("loop")?;

        let block = self.parse_block()?;
        Ok(LoopStatementNode { block })
    }

    fn parse_for_statement(&mut self) -> AstResult<ForStatementNode> {
        self.skip_expected("for")?;
        let it = self.next();
        if it.kind != TokenKind::Identifier {
            todo!();
        }
        let iterator_variable_name = it.value.clone();
        self.skip_expected("in")?;
        let iteratable = self.parse_expression_until(&["{"])?;
        let for_block = self.parse_block()?;
        Ok(ForStatementNode {
            iterator_variable_name,
            iteratable,
            for_block,
        })
    }

    fn parse_iteration(&mut self) -> AstResult<IterationNode> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstBuilderError::new(name_token, "identifier expected"));
        }
        let iteratable_name = name_token.value.clone();

        self.skip_expected(">>")?;

        let info = self.ctx().vars.get(&iteratable_name).unwrap().clone();
        self.ctx().iterators.push(info);
        let block = self.parse_block()?;
        self.ctx().iterators.pop();

        Ok(IterationNode { iteratable_name, block })
    }

    fn parse_variable_assignment(&mut self) -> AstResult<VariableAssignmentNode> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstBuilderError::new(name_token, "identifier expected"));
        }
        let var_name = name_token.value.clone();
        //let info = self.current_function_context.as_mut().unwrap().vars.get(&var_name);
        self.skip_expected("=")?;

        let expression = self.parse_expression_until(&["\n"])?;
        let type_str = expression.type_str.clone();

        let info = match self.ctx().vars.entry(var_name.clone()) {
            Entry::Occupied(occupied) => {
                let info = occupied.into_mut();
                // if info.type_str != type_str {
                //     let s = format!(
                //         "variable assignment with different type than first assignment\nwas {}, became {}",
                //         info.type_str.clone(),
                //         type_str.clone()
                //     );
                //     return Err(ae(&name_token, s));
                // }

                // todo:
                // if info.is_array != is_array {
                //     let s = format!(
                //         "variable assignment with different type than first assignment\nwas {}, became {}",
                //         info.type_str.clone(), type_str.clone()
                //     );
                //     return Err(ae(&name_token, s));
                // }

                info
            }
            Entry::Vacant(vacant) => {
                let info = TypeInfo {
                    is_array: false,
                    type_str: type_str.clone(),
                };
                vacant.insert(info)
            }
        };

        Ok(VariableAssignmentNode {
            name: var_name.clone(),
            rvalue_expression: expression,
        })
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
            return Err(AstBuilderError::new(name_token, "identifier expected"));
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
        let expression = self.parse_expression_until(&[",", "]"])?;
        self.skip_expected("]")?;

        Ok(ArrayBracketsNode { expression })
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
                    let node = AstNode::Group(group_node);
                    ast_nodes.push(node);
                    continue;
                }

                if t.value == "[" {
                    let array_brackets_node = self.parse_array_brackets()?;
                    let node = AstNode::ArrayBrackets(array_brackets_node);
                    ast_nodes.push(node);
                    continue;
                }
            }

            let t = self.next();
            match t.kind {
                TokenKind::LineEnd => {
                    break;
                }
                TokenKind::Identifier => {
                    let node = AstNode::Identifier(IdentifierNode {
                        token: t.clone(),
                        value: t.value.clone(),
                    });
                    ast_nodes.push(node);
                }
                TokenKind::SpecialSymbol => {
                    let node = AstNode::Operator(OperatorNode { value: t.value.clone() });
                    ast_nodes.push(node);
                }
                TokenKind::IntegerLiteral => {
                    let node = AstNode::IntegerLiteral(IntegerLiteralNode { value: t.value.clone() });
                    ast_nodes.push(node);
                }
                TokenKind::FloatLiteral => {
                    let node = AstNode::RealLiteral(RealLiteralNode { value: t.value.clone() });
                    ast_nodes.push(node);
                }
                TokenKind::String => {
                    let node = AstNode::StringLiteral(StringLiteralNode { value: t.value.clone() });
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

        let (node, type_str) = self.tree_node_to_ast_node(&root)?;
        Ok(ExpressionNode {
            type_str,
            root: Box::new(node),
        })
    }

    fn ast_node_list_to_tree_node<'b>(
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
                    let next_priority = *self.tokenizer.priorities.get(&operator.value).unwrap();

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

                AstNode::Group(group_node) => match &tree_node.ast_node {
                    AstNode::Identifier(identifier_node) => {
                        let node = AstNode::FunctionCall(FunctionCallNode {
                            name: identifier_node.value.clone(),
                            params_group: group_node.clone(),
                        });
                        tree_node.ast_node = node;
                        *next_ast_node_index += 1;
                        break;
                    }

                    else_ => {
                        println!("{:?}", tree_node.ast_node);
                        panic!()
                    }
                },

                AstNode::ArrayBrackets(array_brackets_node) => match &tree_node.ast_node {
                    AstNode::Identifier(identifier_node) => {
                        let node = AstNode::ArrayItemAccess(ArrayItemAccessNode {
                            array_name: identifier_node.value.clone(),
                            access_expression: array_brackets_node.expression.clone(),
                        });
                        tree_node.ast_node = node;
                        *next_ast_node_index += 1;
                        break;
                    }

                    else_ => {
                        println!("{:?}", tree_node.ast_node);
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

    fn tree_node_to_ast_node(&self, tree_node: &TreeNode) -> AstResult<(AstNode, String)> {
        fn get_binary_node_type(a: &String, b: &String) -> String {
            if a == b {
                return a.clone();
            }

            if a == "real" && b == "int" || a == "int" && b == "real" {
                return "real".to_string();
            }

            //panic!();
            return "some types".to_string();
        }

        match &tree_node.ast_node {
            AstNode::RealLiteral(_) => {
                return Ok((tree_node.ast_node.clone(), "real".to_string()));
            }

            AstNode::IntegerLiteral(_) => {
                return Ok((tree_node.ast_node.clone(), "int".to_string()));
            }

            AstNode::StringLiteral(_) => {
                return Ok((tree_node.ast_node.clone(), "str".to_string()));
            }

            AstNode::BinaryOperation(b) => {
                return Ok((tree_node.ast_node.clone(), b.type_str.clone()));
            }

            AstNode::Operator(o) => {
                let (left, left_type) = self.tree_node_to_ast_node(&tree_node.childs[0])?;
                let (right, right_type) = self.tree_node_to_ast_node(&tree_node.childs[1])?;
                let mut type_str = get_binary_node_type(&left_type, &right_type);
                let node = AstNode::BinaryOperation(BinaryOperationNode {
                    type_str: type_str.clone(),
                    operation: o.clone(),
                    children: vec![left, right],
                });
                return Ok((node, type_str));
            }

            AstNode::Identifier(identifier_node) => {
                let ctx = self.current_function_context.as_ref().unwrap();

                let var_name = &identifier_node.value;

                if var_name == "it" {
                    let type_info = ctx.iterators.last().unwrap();
                    return Ok((tree_node.ast_node.clone(), type_info.type_str.clone()));
                }

                if var_name == "it_index" {
                    return Ok((tree_node.ast_node.clone(), "int".to_string()));
                }

                if let Some(type_info) = ctx.vars.get(var_name) {
                    return Ok((tree_node.ast_node.clone(), type_info.type_str.clone()));
                }

                if let Some(type_info) = ctx.params.get(&identifier_node.value) {
                    return Ok((tree_node.ast_node.clone(), type_info.type_str.clone()));
                }

                return Ok((tree_node.ast_node.clone(), "unknown".to_string()));

                return Err(AstBuilderError::new(
                    identifier_node.token.clone(),
                    "undeclared identifier",
                ));
            }

            AstNode::Group(group) => {
                if group.expressions.is_empty() {
                    return Err(AstBuilderError::new(group.token.clone(), "empty group"));
                }

                let last = group.expressions.last().unwrap();
                return Ok((tree_node.ast_node.clone(), last.type_str.clone()));
            }

            AstNode::ArrayItemAccess(array_item_access_node) => {
                // if check empty expression {
                //     return Err(AstBuilderError::new(group.token.clone(), "empty group"));
                // }
                let ctx = self.current_function_context.as_ref().unwrap();
                let type_str: String;
                if ctx.vars.contains_key(&array_item_access_node.array_name) {
                    type_str = ctx
                        .vars
                        .get(&array_item_access_node.array_name)
                        .unwrap()
                        .type_str
                        .clone();
                } else if ctx.params.contains_key(&array_item_access_node.array_name) {
                    type_str = ctx
                        .params
                        .get(&array_item_access_node.array_name)
                        .unwrap()
                        .type_str
                        .clone();
                } else {
                    panic!();
                }

                return Ok((tree_node.ast_node.clone(), type_str));
            }

            AstNode::FunctionCall(function_call_node) => {
                return Ok((tree_node.ast_node.clone(), "int".to_string()));
            }

            else_ => {
                assert!(false, "can't detect type");
                return Ok((tree_node.ast_node.clone(), "unknown".to_string()));
            }
        }
    }
}

#[derive(Debug)]
struct TreeNode {
    ast_node: AstNode,
    childs: Vec<TreeNode>,
}

fn ae(t: &Token, s: impl Into<String>) -> AstBuilderError {
    AstBuilderError::new(t.clone(), s.into())
}
