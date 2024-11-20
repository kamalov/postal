#![allow(warnings)]
use std::{vec};

use indexmap::{IndexMap};

use crate::stage01_tokenizer::*;
use crate::stage02_ast_builder::*;

#[derive(Debug)]
pub struct TypeCheckError {
    pub token: Token,
    pub expected: String,
}

impl TypeCheckError {
    pub fn new(token: &Token, s: impl Into<String>) -> TypeCheckError {
        TypeCheckError {
            token: token.clone(),
            expected: s.into(),
        }
    }
}

type TypeCheckResult<T> = Result<T, TypeCheckError>;

#[derive(Clone, Default)]
struct CurrentFunctionContext {
    vars: IndexMap<String, TypeInfo>,
    function_params: IndexMap<String, TypeInfo>,
    iterator_type_list: Vec<TypeInfo>,
    return_type: Option<TypeInfo>,
}

pub struct TypeChecker {
    pub ast_builder: AstBuilder,
    ctx: CurrentFunctionContext,
}

fn get_common_type(a: &TypeInfo, b: &TypeInfo) -> Result<TypeInfo, ()> {
    if a.is_array && b.is_array {
        if a.type_str == b.type_str {
            return Ok(a.clone());
        }
        return Err(());
    }

    if a.is_array != b.is_array {
        return Err(());
    }

    if a.type_str == b.type_str {
        return Ok(a.clone());
    }

    if a.type_str == "real" && b.type_str == "int" || a.type_str == "int" && b.type_str == "real" {
        return Ok(TypeInfo {
            is_array: false,
            type_str: "real".to_string(),
        });
    }

    return Err(());
}

fn can_lift_type_from_to(from: &TypeInfo, to: &TypeInfo) -> bool {
    if from == to {
        return true;
    }

    if from.is_array != to.is_array {
        return false;
    }

    if from.type_str == "int" && to.type_str == "real" {
        return true;
    }

    return false;
}

impl TypeChecker {
    pub fn new(ast_builder: AstBuilder) -> TypeChecker {
        TypeChecker {
            ast_builder,
            ctx: CurrentFunctionContext::default(),
        }
    }

    pub fn build_new_ast_with_types(&mut self) -> TypeCheckResult<Vec<RootNode>> {
        let mut root_nodes = self.ast_builder.root_nodes.clone();

        for root_node in &mut root_nodes {
            match root_node {
                RootNode::Function(function) => {
                    self.process_function(function)?;
                }
                _ => {}
            }
        }

        Ok(root_nodes)
    }

    fn get_function_param_or_var_type(&self, name: &String) -> Option<&TypeInfo> {
        match self.ctx.function_params.get(name) {
            Some(type_info) => Some(type_info),
            None => match self.ctx.vars.get(name) {
                Some(type_info) => Some(type_info),
                None => None,
            },
        }
    }

    fn process_function(&mut self, function: &mut Function) -> TypeCheckResult<()> {
        self.ctx = CurrentFunctionContext {
            vars: IndexMap::new(),
            function_params: function.decl.params.clone(),
            iterator_type_list: vec![],
            return_type: function.decl.return_type.clone(),
        };

        self.process_block_statements(&function.body)?;

        function.vars = std::mem::take(&mut self.ctx.vars);
        function.decl.return_type = std::mem::take(&mut self.ctx.return_type);

        let decl = self.ast_builder.fn_declarations.get_mut(&function.decl.name).unwrap();
        decl.return_type = function.decl.return_type.clone();

        self.ctx = CurrentFunctionContext::default();

        Ok(())
    }

    fn get_expression_type(&self, expression: &Expression) -> TypeCheckResult<TypeInfo> {
        match &*expression.kind {
            ExpressionKind::IntegerLiteral(..) => {
                return Ok(TypeInfo {
                    type_str: "int".to_string(),
                    ..TypeInfo::default()
                })
            }
            ExpressionKind::RealLiteral(..) => {
                return Ok(TypeInfo {
                    type_str: "real".to_string(),
                    ..TypeInfo::default()
                })
            }
            ExpressionKind::StringLiteral(..) => {
                return Ok(TypeInfo {
                    type_str: "str".to_string(),
                    ..TypeInfo::default()
                })
            }
            ExpressionKind::FunctionCall(function_call) => {
                match self.ast_builder.fn_declarations.get(&function_call.name) {
                    Some(fn_declaration) => match &fn_declaration.return_type {
                        Some(return_type) => {
                            return Ok(return_type.clone());
                        }
                        None => {
                            return Err(TypeCheckError::new(
                                &expression.token,
                                format!("can't figure return type of '{}' function call, please specify return type explicitly",
                                function_call.name)));
                        }
                    },
                    None => {
                        return Err(TypeCheckError::new(&expression.token, "no such function"));
                    }
                }

                // todo: check formal and actual params
            }
            ExpressionKind::Identifier(identifier_name) => {
                //
                match identifier_name.as_str() {
                    "it" => {
                        if self.ctx.iterator_type_list.is_empty() {
                            return Err(TypeCheckError::new(
                                &expression.token,
                                "Keyword 'it' is reserved and can be used only in iterations",
                            ));
                        }
                        let type_info = self.ctx.iterator_type_list.last().unwrap();
                        return Ok(type_info.clone());
                    }
                    "idx" => {
                        return Ok(TypeInfo {
                            type_str: String::from("int"),
                            ..TypeInfo::default()
                        })
                    }
                    else_ => {
                        //
                        match self.get_function_param_or_var_type(&identifier_name) {
                            Some(type_info) => {
                                return Ok(type_info.clone());
                            }
                            None => {
                                return Err(TypeCheckError::new(&expression.token, "undeclared variable"));
                            }
                        }
                    }
                }
            }
            ExpressionKind::BinaryOperation { operation, left, right } => {
                let left_side_type_info = self.get_expression_type(&left)?;
                
                if operation == "." {
                    if let ExpressionKind::Identifier(right_identifier_name) = &*right.kind {
                        match self.ast_builder.records.get(&left_side_type_info.type_str) {
                            Some(record) => {
                                let field_name = right_identifier_name;
                                match record.fields.get(field_name) {
                                    Some(type_info) => return Ok(type_info.clone()),
                                    None => {
                                        return Err(TypeCheckError::new(
                                            &right.token,
                                            format!("unknown field: '{field_name}'"),
                                        ));
                                    }
                                }
                            }
                            None => {
                                return Err(TypeCheckError::new(
                                    &expression.token,
                                    format!("unknown type: '{left_side_type_info}'"),
                                ));
                            }
                        }
                        Ok(left_side_type_info)
                    } else {
                        return Err(TypeCheckError::new(
                            &expression.token,
                            format!("must be identifier after .: '{right:?}'"),
                        ));
                    }
                } else {
                    let right_side_type_info = self.get_expression_type(&right)?;
                    match get_common_type(&left_side_type_info, &right_side_type_info) {
                        Ok(common_type) => {
                            return Ok(common_type);
                        }
                        Err(_) => {
                            return Err(TypeCheckError::new(
                                &expression.token,
                                format!("incompatible types: '{left_side_type_info}' and '{right_side_type_info}'"),
                            ));
                        }
                    }
                }
            }
            ExpressionKind::Group(group_expressions) => {
                if group_expressions.is_empty() {
                    return Err(TypeCheckError::new(&expression.token, "empty group"));
                }

                for expression in group_expressions {
                    self.get_expression_type(expression)?;
                }

                let last_type_info = self.get_expression_type(group_expressions.last().unwrap())?;

                return Ok(last_type_info);
            }
            ExpressionKind::ArrayItemAccess { array_name, access_expression } => {
                let type_info = self.get_expression_type(&access_expression)?;

                if type_info.is_array || type_info.type_str != "int" {
                    return Err(TypeCheckError::new(
                        &expression.token,
                        format!("incorrect index type '{type_info}'"),
                    ));
                }

                let type_info = match self.get_function_param_or_var_type(&array_name) {
                    Some(type_info) => type_info,
                    None => {
                        return Err(TypeCheckError::new(&expression.token, format!("undeclared array '{array_name}'")));
                    }
                };

                if !type_info.is_array {
                    return Err(TypeCheckError::new(&expression.token, format!("not an array: '{array_name}'")));
                }

                return Ok(type_info.clone());
            }
            _ => {
                //ExpressionNode::ArrayBrackets(array_brackets_node) => panic!(),
                //ExpressionNode::Operator(operator_node) => panic!(),
                println!("{expression:?}");
                panic!()
            }
        }
    }

    fn process_block_statements(&mut self, block: &Block) -> TypeCheckResult<()> {
        for statement in &block.statements {
            match statement {
                StatementNode::If(if_statement) => {
                    for (_, block) in &if_statement.if_blocks {
                        self.process_block_statements(block);
                    }

                    if let Some(block) = &if_statement.else_block {
                        self.process_block_statements(&block);
                    }
                }
                StatementNode::Loop(loop_statement) => {
                    self.process_block_statements(&loop_statement.block);
                }
                StatementNode::Return(expression) => {
                    let return_type = self.get_expression_type(expression)?;
                    match &self.ctx.return_type {
                        Some(current_return_type) => match get_common_type(current_return_type, &return_type) {
                            Ok(return_type) => {
                                self.ctx.return_type = Some(return_type);
                            }
                            Err(_) => {
                                return Err(TypeCheckError::new(&expression.token, "return type differs from previous"));
                            }
                        },
                        None => self.ctx.return_type = Some(return_type),
                    }
                }
                StatementNode::Iteration((token, iteratable_name, block)) => {
                    //
                    match self.get_function_param_or_var_type(iteratable_name) {
                        Some(type_info) => {
                            self.ctx.iterator_type_list.push(type_info.clone());
                            let block = self.process_block_statements(block)?;
                            self.ctx.iterator_type_list.pop();
                        }
                        None => {
                            return Err(TypeCheckError::new(
                                token,
                                format!("unknown iteratable: '{iteratable_name}'"),
                            ));
                        }
                    }
                }
                StatementNode::VariableAssignment(variable_assignment) => {
                    let name = variable_assignment.name.clone();
                    let var_type = self.get_expression_type(&variable_assignment.rvalue_expression)?;
                    match self.ctx.vars.entry(name) {
                        indexmap::map::Entry::Occupied(mut occupied_entry) => {
                            let current_var_type = occupied_entry.get().clone();
                            match get_common_type(&current_var_type, &var_type) {
                                Ok(new_type) => {
                                    // todo: handle explicit type specifier case in var declaration
                                    *occupied_entry.get_mut() = new_type
                                }
                                Err(_) => {
                                    return Err(TypeCheckError::new(
                                        &variable_assignment.token,
                                        format!("incompatible types: '{current_var_type}' and '{var_type}'"),
                                    ));
                                }
                            }
                        }
                        indexmap::map::Entry::Vacant(vacant_entry) => {
                            vacant_entry.insert(var_type);
                        }
                    }
                }
                StatementNode::VariableDeclaration((token, name, type_info)) => {
                    match self.get_function_param_or_var_type(&name) {
                        Some(type_info) => {
                            return Err(TypeCheckError::new(
                                token,
                                format!("variable already defined: '{name}'"),
                            ));
                        }
                        None => {
                            self.ctx.vars.insert(name.clone(), type_info.clone());
                        }
                    }
                }
                StatementNode::Assignment(Assignment { token, lvalue, rvalue }) => {
                    let left_side_type_info = self.get_expression_type(lvalue)?;
                    let right_side_type_info = self.get_expression_type(rvalue)?;
                    if !can_lift_type_from_to(&right_side_type_info, &left_side_type_info) {
                        return Err(TypeCheckError::new(
                            token,
                            format!("incompatible types, can't assign to '{left_side_type_info}' from '{right_side_type_info}'"),
                        ));
                    }
                }
                StatementNode::FunctionCall(function_call) => {
                    //todo!();
                }

                StatementNode::Break() => {}
                StatementNode::Comment(_) => {}
                _ => {
                    panic!();
                }
            }
        }

        Ok(())
    }
}
