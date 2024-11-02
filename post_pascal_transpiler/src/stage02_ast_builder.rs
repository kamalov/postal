#![allow(warnings)]
use std::collections::hash_map::Entry;
use std::collections::{HashMap, HashSet};
use std::env::var;
use std::fmt::Write;
use std::fs::{read_dir, read_to_string};
use std::slice::Iter;
use std::{array, default, fs, mem};
use to_vec::ToVec;

use crate::stage01_tokenizer::*;

#[derive(Debug, Clone)]
pub struct ProgramNode {
    pub children: Vec<AstNode>,
}

#[derive(Debug, Clone)]
pub struct FunctionNode {
    pub name: String,
    pub body: BlockNode,
    pub vars: HashMap<String, TypeInfo>,
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
    //pub var_type: String,
    pub rvalue_expression: ExpressionNode,
}

#[derive(Debug, Clone)]
pub struct VariableDeclarationNode {
    pub name: String,
    //pub var_type: String,
}

#[derive(Debug, Clone)]
pub struct FunctionCallNode {
    pub function_name: String,
    pub params: Vec<FunctionParamNode>,
}

#[derive(Debug, Clone)]
pub struct IntegerLiteralNode {
    pub value: String,
}

#[derive(Debug, Clone)]
pub struct FloatLiteralNode {
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
    pub var_name: String,
    pub block: BlockNode,
}

#[derive(Debug, Clone)]
pub enum AstNode {
    Program(ProgramNode),
    Function(FunctionNode),
    Block(BlockNode),
    IfStatement(IfStatementNode),
    ForStatement(ForStatementNode),
    LoopStatement(LoopStatementNode),
    Iteration(IterationNode),
    BreakStatement(),
    FunctionCall(FunctionCallNode),
    VariableAssignment(VariableAssignmentNode),
    VariableDeclaration(VariableDeclarationNode),
    IntegerLiteral(IntegerLiteralNode),
    FloatLiteral(FloatLiteralNode),
    StringLiteral(StringLiteralNode),
    Identifier(IdentifierNode),
    Operator(OperatorNode),
    Comment(CommentNode),
    BinaryOperation(BinaryOperationNode),
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

//#[derive(Default)]
pub struct AstBuilder<'a> {
    ti: usize,
    current_function_vars: HashMap<String, TypeInfo>,
    pub tokenizer: &'a Tokenizer,
    pub tokens: Vec<Token>,
    pub root: ProgramNode,
}

impl<'a> AstBuilder<'a> {
    pub fn new(tokenizer: &Tokenizer) -> AstBuilder {
        AstBuilder {
            ti: 0,
            current_function_vars: HashMap::new(),
            tokenizer: tokenizer,
            tokens: tokenizer.tokens.clone(),
            root: ProgramNode { children: vec![] },
        }
    }

    fn peek(&self) -> &Token {
        if self.ti < self.tokens.len() {
            &self.tokens[self.ti]
        } else {
            panic!()
        }
    }

    fn peek_second(&self) -> &Token {
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

    fn skip_line_end_tokens(&mut self) {
        while self.tokens[self.ti].kind == TokenKind::LineEnd {
            self.ti += 1;
        }
    }

    fn skip_with_check(&mut self, check: &str) -> AstResult<()> {
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
        if self.peek().is_eof() {
            todo!();
        };

        let t = self.peek();
        //if t.kind == TokenKind::Special && t.value == check {
        if t.value == check {
            self.skip_with_check(check);
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
        let mut root = ProgramNode { children: vec![] };

        loop {
            self.skip_line_end_tokens();

            let t = self.peek();
            if t.is_eof() {
                break;
            };

            match t.kind {
                TokenKind::Keyword => {
                    if t.value == "fn" {
                        let fun = self.parse_function()?;
                        root.children.push(AstNode::Function(fun));
                    }
                }
                _ => todo!(),
            }
        }

        self.root = root;
        Ok(())
    }

    fn parse_function(&mut self) -> AstResult<FunctionNode> {
        self.skip_with_check("fn")?;

        let name_token = self.next();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstBuilderError::new(name_token.clone(), "identifier"));
        }

        let name = name_token.value.clone();
        let block = self.parse_function_body()?;
        let vars = mem::take(&mut self.current_function_vars);

        Ok(FunctionNode { name, body: block, vars })
    }

    fn parse_function_body(&mut self) -> AstResult<BlockNode> {
        self.skip_line_end_tokens();
        self.skip_with_check("begin")?;
        let mut statements = vec![];
        loop {
            self.skip_line_end_tokens();

            let t = self.peek();

            if t.kind == TokenKind::Keyword && t.value == "end" {
                self.skip_with_check("end")?;
                break;
            }

            let statement = self.parse_statement()?;
            statements.push(statement);
        }

        Ok(BlockNode { statements })
    }

    fn parse_block(&mut self) -> AstResult<BlockNode> {
        self.skip_with_check("{")?;
        let mut statements = vec![];
        loop {
            self.skip_line_end_tokens();

            let t = self.peek();
            if t.kind == TokenKind::Special && t.value == "}" {
                self.skip_with_check("}")?;
                break;
            }

            let statement = self.parse_statement()?;
            statements.push(statement);
        }

        Ok(BlockNode { statements })
    }

    fn parse_statement(&mut self) -> AstResult<AstNode> {
        let t = self.peek();
        //println!("{:?}", t);
        match t.kind {
            TokenKind::Identifier => {
                let t = self.peek_second();
                match t.value.as_str() {
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

                    _ => {
                        panic!()
                    }
                }
            }

            TokenKind::Keyword => {
                if t.value == "if" {
                    let f = self.parse_if_statement()?;
                    return Ok(AstNode::IfStatement(f));
                }

                if t.value == "for" {
                    let f = self.parse_for_statement()?;
                    return Ok(AstNode::ForStatement(f));
                }

                if t.value == "loop" {  
                    let f = self.parse_loop_statement()?;
                    return Ok(AstNode::LoopStatement(f));
                }

                if t.value == "break" {
                    self.skip_with_check("break")?;
                    return Ok(AstNode::BreakStatement());
                }

                todo!();
            }

            TokenKind::Comment => {
                let value = t.value.clone();
                self.skip();
                Ok(AstNode::Comment(CommentNode { value }))
            }
            _ => todo!(),
        }
    }

    fn parse_if_statement(&mut self) -> AstResult<IfStatementNode> {
        self.skip_with_check("if")?;
        
        let mut if_blocks: Vec<(ExpressionNode, BlockNode)> = Vec::new();
        let mut else_block = None;

        let condition = self.parse_expression(&["{"])?;
        let block = self.parse_block()?;
        if_blocks.push((condition, block));

        loop {
            if self.try_skip("else") {
                if self.try_skip("if") {
                    let condition = self.parse_expression(&["{"])?;
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

        Ok(IfStatementNode {
            if_blocks,
            else_block,
        })
    }

    fn parse_loop_statement(&mut self) -> AstResult<LoopStatementNode> {
        self.skip_with_check("loop")?;
        
        let block = self.parse_block()?;
        Ok(LoopStatementNode { block })
    }
 
    fn parse_for_statement(&mut self) -> AstResult<ForStatementNode> {
        self.skip_with_check("for")?;
        let it = self.next();
        if it.kind != TokenKind::Identifier {
            todo!();
        }
        let iterator_variable_name = it.value.clone();
        self.skip_with_check("in")?;
        let iteratable = self.parse_expression(&["{"])?;
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
        let var_name = name_token.value.clone();

        self.skip_with_check(">>")?;

        let block = self.parse_block()?;
        Ok(IterationNode {
            var_name,
            block
        })
    }

    fn parse_variable_assignment(&mut self) -> AstResult<VariableAssignmentNode> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstBuilderError::new(name_token, "identifier expected"));
        }
        let var_name = name_token.value.clone();
        let info = self.current_function_vars.get(&var_name);
        self.skip_with_check("=")?;

        let expression = self.parse_expression(&["\n"])?;
        let type_str = expression.type_str.clone();

        let info = match self.current_function_vars.entry(var_name.clone()) {
            Entry::Occupied(o) => {
                let info = o.into_mut();
                if info.type_str != type_str {
                    let s = format!(
                        "variable assignment with different type than first assignment\nwas {}, became {}",
                        info.type_str.clone(), type_str.clone()
                    );
                    return Err(ae(&name_token, s));
                }

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
            Entry::Vacant(v) => {
                let info = TypeInfo {
                    is_array: false,
                    type_str: type_str.clone(),
                };
                v.insert(info)
            }
        };

        Ok(VariableAssignmentNode {
            name: var_name.clone(),
            rvalue_expression: expression,
        })
    }

    fn parse_variable_declaration(&mut self) -> AstResult<VariableDeclarationNode> {
        let name_token = self.next().clone();
        if name_token.kind != TokenKind::Identifier {
            return Err(AstBuilderError::new(name_token, "identifier expected"));
        }
        let var_name = name_token.value.clone();
        self.skip_with_check(":")?;

        let mut is_array: bool;
        let mut type_str: String;
        if self.peek().value == "[" {
            is_array = true;
            self.skip_with_check("[")?;
            type_str = self.next().value.clone();
            self.skip_with_check("]")?;
        } else {
            is_array = false;
            type_str = self.next().value.clone();
        }

        let info = self.current_function_vars.get(&var_name);
        let info = match self.current_function_vars.entry(var_name.clone()) {
            Entry::Occupied(o) => {
                let info = o.into_mut();
                let s = format!(
                    "variable already defined {}",
                    var_name.clone()
                );
                return Err(ae(&name_token, s));
            }
            Entry::Vacant(v) => {
                let info = TypeInfo {
                    is_array,
                    type_str: type_str.clone(),
                };
                v.insert(info)
            }
        };

        Ok(VariableDeclarationNode {
            name: var_name.clone(),
        })
    }

    fn parse_function_call(&mut self) -> AstResult<FunctionCallNode> {
        let t = self.next();
        let function_name = t.value.clone();
        let params = self.parse_function_call_params()?;
        Ok(FunctionCallNode {
            function_name,
            params,
        })
    }

    fn parse_function_call_params(&mut self) -> AstResult<Vec<FunctionParamNode>> {
        self.skip_with_check("(")?;
        let mut params = vec![];
        loop {
            if self.try_skip(")") {
                break;
            }

            let expression = self.parse_expression(&[",", ")"])?;
            let p = FunctionParamNode { expression };
            params.push(p);

            self.try_skip(",");
        }

        Ok(params)
    }

    fn parse_expression(&mut self, terminators: &[&str]) -> AstResult<ExpressionNode> {

        fn parse_expression_part<'b>(
            builder: &mut AstBuilder,
            nodes: &'b Vec<AstNode>,
            index: &mut usize,
            node: TreeNode<'b>,
            priority: usize,
        ) -> TreeNode<'b> {
            let mut node = node;
            let mut priority = priority;
            loop {
                if *index >= nodes.len() {
                    break;
                }
    
                let next_op = &nodes[*index];
                if let AstNode::Operator(o) = next_op {
                    let next_priority = *builder.tokenizer.priorities.get(&o.value).unwrap();
    
                    if next_priority < priority {
                        break;
                    }
    
                    let next_r = &nodes[*index + 1];
                    let next_r = TreeNode {
                        ast_node: next_r,
                        childs: vec![],
                    };
                    *index += 2;
                    let next_r = parse_expression_part(builder, nodes, index, next_r, next_priority);
                    let mut childs = vec![];
                    childs.push(node);
                    childs.push(next_r);
                    node = TreeNode {
                        ast_node: &next_op,
                        childs,
                    }
                } else {
                    //println!("{:?}", next_op);
                    todo!();
                }
            }
    
            node
        }
        
        let mut nodes = vec![];
        loop {
            let t = self.peek();
            if t.kind == TokenKind::Special {
                if terminators.contains(&t.value.as_str()) {
                    break;
                }
            }

            let t = self.next();
            match t.kind {
                TokenKind::LineEnd => {
                    break;
                }
                TokenKind::Identifier => {
                    let n = AstNode::Identifier(IdentifierNode {
                        token: t.clone(),
                        value: t.value.clone(),
                    });
                    nodes.push(n);
                }
                TokenKind::Special => {
                    let n = AstNode::Operator(OperatorNode {
                        value: t.value.clone(),
                    });
                    nodes.push(n);
                }
                TokenKind::Integer => {
                    let n = AstNode::IntegerLiteral(IntegerLiteralNode {
                        value: t.value.clone(),
                    });
                    nodes.push(n);
                }
                TokenKind::Float => {
                    let n = AstNode::FloatLiteral(FloatLiteralNode {
                        value: t.value.clone(),
                    });
                    nodes.push(n);
                }
                TokenKind::String => {
                    let n = AstNode::StringLiteral(StringLiteralNode {
                        value: t.value.clone(),
                    });
                    nodes.push(n);
                }
                _ => (),
            }
        }

        let mut root = TreeNode {
            ast_node: &nodes[0],
            childs: vec![],
        };

        let mut index = 1;
        loop {
            if index < nodes.len() {
                root = parse_expression_part(self, &nodes, &mut index, root, 0);
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

    fn tree_node_to_ast_node(&self, tree_node: &TreeNode) -> AstResult<(AstNode, String)> {
        fn get_binary_type(a: &String, b: &String) -> String {
            if a == b {
                return a.clone();
            }

            if a == "float" && b == "int" || a == "int" && b == "float" {
                return "float".to_string();
            }

            return "incompatible types".to_string();
        }

        match tree_node.ast_node {
            AstNode::Operator(o) => {
                let (left, left_type) = self.tree_node_to_ast_node(&tree_node.childs[0])?;
                let (right, right_type) = self.tree_node_to_ast_node(&tree_node.childs[1])?;
                let mut type_str = get_binary_type(&left_type, &right_type);
                let node = AstNode::BinaryOperation(BinaryOperationNode {
                    type_str: type_str.clone(),
                    operation: o.clone(),
                    children: vec![left, right],
                });
                return Ok((node, type_str));
            }
            AstNode::FloatLiteral(_) => {
                return Ok((tree_node.ast_node.clone(), "float".to_string()));
            }
            AstNode::IntegerLiteral(_) => {
                return Ok((tree_node.ast_node.clone(), "int".to_string()));
            }
            AstNode::BinaryOperation(b) => {
                return Ok((tree_node.ast_node.clone(), b.type_str.clone()));
            }
            AstNode::Identifier(v) => {
                if v.value == "it" || v.value == "it_index" {
                    return Ok((tree_node.ast_node.clone(), "".to_string()));
                } else if let Some(info) = self.current_function_vars.get(&v.value) {
                    return Ok((tree_node.ast_node.clone(), info.type_str.clone()));
                } else {
                    return Err(AstBuilderError::new(
                        v.token.clone(),
                        "undeclared identifier",
                    ));
                }
            }
            else_ => {
                return Ok((tree_node.ast_node.clone(), "unknown".to_string()));
            }
        }
    }
}

#[derive(Debug)]
struct TreeNode<'a> {
    ast_node: &'a AstNode,
    childs: Vec<TreeNode<'a>>,
}

fn ae(t: &Token, s: impl Into<String>) -> AstBuilderError {
    AstBuilderError::new(t.clone(), s.into())
}