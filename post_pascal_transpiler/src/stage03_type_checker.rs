use std::vec;
use indexmap::IndexMap;
use std::collections::hash_map::Entry;
use std::collections::HashMap;
use crate::utils::*;
use crate::type_info::*;
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

#[derive(Default)]
struct CurrentFunctionContext {
    vars: IndexMap<String, TypeInfo>,
    function_params: IndexMap<String, TypeInfo>,
    iterator_type_list: Vec<TypeInfo>,
    return_type: Option<TypeInfo>,
}

pub struct TypeChecker {
    pub ast_builder: AstBuilder,
    pub fn_declarations: HashMap<String, FnDeclaration>,
    ctx: CurrentFunctionContext,
}

impl TypeChecker {
    pub fn new(ast_builder: AstBuilder) -> TypeChecker {
        TypeChecker {
            ast_builder,
            fn_declarations: HashMap::new(),
            ctx: CurrentFunctionContext::default(),
        }
    }

    pub fn build_new_ast_with_types(&mut self) -> TypeCheckResult<AstBuilder> {
        let mut ast_builder = self.ast_builder.clone();
        
        for (idx, root_node) in ast_builder.root_nodes.iter_mut().enumerate() {
            match root_node {
                RootNode::Function(function) => {
                    self.preprocess_function(function, idx)?;
                }
                _ => {}
            }
        }

        for root_node in &mut ast_builder.root_nodes {
            match root_node {
                RootNode::Function(function) => {
                    self.process_function(function)?;
                }
                _ => {}
            }
        }

        Ok(ast_builder)
    }

    fn preprocess_function(&mut self, fun: &mut Function, idx: usize) -> TypeCheckResult<()> {
        for (_, type_info) in &mut fun.declaration.params {
            let is_generic = intersects(&fun.declaration.generic_params, &type_info.get_type_str_list());
            type_info.is_generic = is_generic;
        }

        if let Some(ref mut type_info) = fun.declaration.return_type {
            let is_generic = intersects(&fun.declaration.generic_params, &type_info.get_type_str_list());
            type_info.is_generic = is_generic;
        }

        let fn_name = &fun.declaration.name;
        if self.fn_declarations.contains_key(fn_name) {
            return Err(TypeCheckError::new(&fun.declaration.token, "duplicate function name"));
        } else {
            self.fn_declarations.insert(fn_name.clone(), fun.declaration.clone());
        }

        Ok(())
    }

    fn process_function(&mut self, function: &mut Function) -> TypeCheckResult<()> {
        if function.is_external {
            return Ok(());
        }

        self.ctx = CurrentFunctionContext {
            vars: IndexMap::new(),
            function_params: function.declaration.params.clone(),
            iterator_type_list: vec![],
            return_type: function.declaration.return_type.clone(),
        };

        self.process_block_statements(&mut function.body)?;

        function.vars = std::mem::take(&mut self.ctx.vars);
        function.declaration.return_type = std::mem::take(&mut self.ctx.return_type);

        if function.declaration.return_type.is_some() {
            let decl = self.fn_declarations.get_mut(&function.declaration.name).unwrap();
            if decl.return_type.is_none() {
                decl.return_type = function.declaration.return_type.clone();
            }
        }

        self.ctx = CurrentFunctionContext::default();

        Ok(())
    }

    fn get_function_declaration(&self, name: &String) -> Option<&FnDeclaration> {
        if let Some(decl) = self.fn_declarations.get(name) {
            return Some(decl);
        } else {
            return None;
        }
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

    fn get_expression_type(&self, expression: &Expression) -> TypeCheckResult<TypeInfo> {
        match &*expression.kind {
            ExpressionKind::IntegerLiteral(..) => {
                return Ok(TypeInfo::new_scalar("int"));
            }
            ExpressionKind::RealLiteral(..) => {
                return Ok(TypeInfo::new_scalar("real"));
            }
            ExpressionKind::StringLiteral(..) => {
                return Ok(TypeInfo::new_scalar("str"));
            }
            ExpressionKind::FunctionCall(function_call) => {
                let mut actual_param_types = vec![];
                for param in &function_call.params {
                    let type_info = self.get_expression_type(param)?;
                    actual_param_types.push(type_info);
                }

                let decl = if let Some(decl) = self.get_function_declaration(&function_call.name) {
                    decl
                } else {
                    return Err(TypeCheckError::new(&expression.token, "no such function"));
                };

                if decl.params.len() != actual_param_types.len() {
                    return Err(TypeCheckError::new(&expression.token, "wrong number of parameters in function call"));
                }
                // todo: check formal and actual params compatibility

                if let Some(return_type) = &decl.return_type {
                    if !return_type.is_generic {
                        return Ok(return_type.clone());
                    }

                    todo!();
                    for (idx, (_, param_type)) in decl.params.iter().enumerate() {
                        let types = param_type.get_type_str_list();
                        let type_str = if types.len() >= 2 {
                            todo!()
                        } else {
                            types[0]
                        };
                    }
                }

                return Err(TypeCheckError::new(
                    &expression.token,
                    format!("can't figure return type of '{}' function call, please specify return type explicitly", function_call.name),
                ));
            }
            ExpressionKind::Identifier(identifier_name) => {
                //
                match identifier_name.as_str() {
                    "it" => {
                        if self.ctx.iterator_type_list.is_empty() {
                            return Err(TypeCheckError::new(&expression.token, "Keyword 'it' is reserved and can be used only in iterations"));
                        }
                        let type_info = self.ctx.iterator_type_list.last().unwrap();
                        return Ok(TypeInfo::new_scalar(type_info.get_array_type_str()));
                    }
                    "idx" => {
                        return Ok(TypeInfo::new_scalar("int"));
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
                        match self.ast_builder.records.get(left_side_type_info.get_scalar_type_str()) {
                            Some(record) => {
                                let field_name = right_identifier_name;
                                match record.fields.get(field_name) {
                                    Some(type_info) => return Ok(type_info.clone()),
                                    None => {
                                        return Err(TypeCheckError::new(&right.token, format!("unknown field: '{field_name}'")));
                                    }
                                }
                            }
                            None => {
                                return Err(TypeCheckError::new(&expression.token, format!("unknown type: '{left_side_type_info}'")));
                            }
                        }
                        return Ok(left_side_type_info);
                    } else {
                        return Err(TypeCheckError::new(&expression.token, format!("must be identifier after .: '{right:?}'")));
                    }
                }

                let right_side_type_info = self.get_expression_type(&right)?;

                if operation == ".." {
                    if !left_side_type_info.is_scalar_type_str("int") {
                        return Err(TypeCheckError::new(&left.token, format!("left side of range must be 'int': found '{left_side_type_info}'")));
                    }

                    if !right_side_type_info.is_scalar_type_str("int") {
                        return Err(TypeCheckError::new(&left.token, format!("right side of range must be 'int': found '{right_side_type_info}'")));
                    }

                    return Ok(TypeInfo::new_array("int"));
                }

                match get_common_type(&left_side_type_info, &right_side_type_info) {
                    Ok(common_type) => {
                        if ["<", "<=", ">", ">=", "=", "<>"].contains(&operation.as_str()) {
                            return Ok(TypeInfo::new_scalar("int"));
                        }
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
            ExpressionKind::ArrayItemAccess { array_expression, access_expression } => {
                let array_type_info = self.get_expression_type(&array_expression)?;

                if !array_type_info.is_array() {
                    return Err(TypeCheckError::new(&expression.token, format!("not an array")));
                }

                let accessor_type_info = self.get_expression_type(&access_expression)?;

                if !accessor_type_info.is_scalar_type_str("int") {
                    return Err(TypeCheckError::new(&expression.token, format!("incorrect index type '{accessor_type_info}'")));
                }

                return Ok(TypeInfo::new_scalar(array_type_info.get_array_type_str()));
            }
            ExpressionKind::ArrayInitializer(identifier) => {
                let type_str = identifier; 
                let type_info = TypeInfo::new_array(type_str);
                if is_builtin_type(&type_str) || self.ast_builder.records.contains_key(type_str) {
                    return Ok(type_info);
                }
                return Err(TypeCheckError::new(&expression.token, format!("unknown type: '{type_str}'")));
            }
            ExpressionKind::HashMapInitializer(key_type_str, value_type_str) => {
                if !is_builtin_type(key_type_str) && !self.ast_builder.records.contains_key(key_type_str) {
                    return Err(TypeCheckError::new(&expression.token, format!("unknown type: '{key_type_str}'")));
                }
                let type_info = TypeInfo::new_hashmap(key_type_str.clone(), value_type_str.clone());
                return Ok(type_info);
            }
            ExpressionKind::ObjectInitializer(record_name) => {
                //
                match self.ast_builder.records.get(record_name) {
                    Some(record) => {
                        return Ok(TypeInfo::new_scalar(&record.name));
                    }
                    None => {
                        return Err(TypeCheckError::new(&expression.token, format!("unknown type: '{record_name}'")));
                    }
                }
            }
            _ => {
                //ExpressionNode::ArrayBrackets(array_brackets_node) => panic!(),
                //ExpressionNode::Operator(operator_node) => panic!(),
                println!("{expression:?}");
                panic!()
            }
        }
    }

    fn process_block_statements(&mut self, block: &mut Block) -> TypeCheckResult<()> {
        for statement in &mut block.statements {
            match statement {
                Statement::If(if_statement) => {
                    for (if_expression, block) in &mut if_statement.if_blocks {
                        let expression = self.get_expression_type(&if_expression)?;
                        self.process_block_statements(block)?;
                    }

                    if let Some(block) = &mut if_statement.else_block {
                        self.process_block_statements(block)?;
                    }
                }
                Statement::Loop(loop_statement) => {
                    self.process_block_statements(&mut loop_statement.block)?;
                }
                Statement::Return(expression_option) => {
                    //
                    match expression_option {
                        None => {}
                        Some(expression) => {
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
                    }
                }
                Statement::Iteration(IterationStatement { token, iterable_name, block }) => {
                    //
                    match self.get_function_param_or_var_type(iterable_name) {
                        Some(type_info) => {
                            self.ctx.iterator_type_list.push(type_info.clone());
                            let block = self.process_block_statements(block)?;
                            self.ctx.iterator_type_list.pop();
                        }
                        None => {
                            return Err(TypeCheckError::new(token, format!("unknown iteratable: '{iterable_name}'")));
                        }
                    }
                }
                Statement::For(ForStatement { iterable_expression, block }) => {
                    //
                    let type_info = self.get_expression_type(&iterable_expression)?;
                    iterable_expression.type_info = Some(type_info.clone());
                    self.ctx.iterator_type_list.push(type_info);
                    let block = self.process_block_statements(block)?;
                    self.ctx.iterator_type_list.pop();
                }
                Statement::VariableAssignment(variable_assignment) => {
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
                                    return Err(TypeCheckError::new(&variable_assignment.token, format!("incompatible types: '{current_var_type}' and '{var_type}'")));
                                }
                            }
                        }
                        indexmap::map::Entry::Vacant(vacant_entry) => {
                            vacant_entry.insert(var_type);
                        }
                    }
                }
                Statement::VariableDeclaration((token, name, type_info)) => {
                    //
                    match self.get_function_param_or_var_type(&name) {
                        Some(type_info) => {
                            return Err(TypeCheckError::new(token, format!("variable already defined: '{name}'")));
                        }
                        None => {
                            self.ctx.vars.insert(name.clone(), type_info.clone());
                        }
                    }
                }
                Statement::Assignment(Assignment { token, lvalue, rvalue }) => {
                    let left_side_type_info = self.get_expression_type(lvalue)?;
                    let right_side_type_info = self.get_expression_type(rvalue)?;
                    if !can_lift_type_from_to(&right_side_type_info, &left_side_type_info) {
                        return Err(TypeCheckError::new(
                            token,
                            format!("incompatible types, can't assign to '{left_side_type_info}' from '{right_side_type_info}'"),
                        ));
                    }
                }
                Statement::FunctionCall(function_call) => {
                    let mut new_params = vec![];
                    for param in &function_call.params {
                        let type_info = self.get_expression_type(param)?;
                        let mut new_param = param.clone();
                        new_param.type_info = Some(type_info);
                        new_params.push(new_param);
                    }
                    function_call.params = new_params;
                }
                Statement::Break() => {}
                Statement::Continue() => {}
                Statement::Comment(_) => {}
                _ => panic!()
            }
        }

        Ok(())
    }
}
