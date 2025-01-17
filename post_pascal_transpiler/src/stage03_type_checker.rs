use crate::stage01_tokenizer::*;
use crate::stage02_ast_builder::*;
use crate::type_info;
use crate::type_info::*;
use crate::utils::*;
use indexmap::IndexMap;
use std::collections::hash_map::Entry;
use std::collections::HashMap;
use std::vec;

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
        self.fn_declarations.get(name)
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

    fn fill_function_call_type_info(&mut self, function_call: &mut FunctionCall, token: &Token) -> TypeCheckResult<TypeInfo> {
        let mut actual_param_types = vec![];
        for expr in &mut function_call.params {
            let type_info = self.fill_expression_type(expr)?;
            expr.type_info = Some(type_info.clone());
            actual_param_types.push(type_info);
        }

        if is_builtin_function(&function_call.name) {
            return Ok(TypeInfo::new_scalar("none"));
        }

        let decl = if let Some(decl) = self.get_function_declaration(&function_call.name) {
            decl
        } else {
            return Err(TypeCheckError::new(&token, "no such function"));
        };

        if decl.params.len() != actual_param_types.len() {
            return Err(TypeCheckError::new(&token, "wrong number of parameters in function call"));
        }

        let mut generic_type_str_to_actual_type_str = HashMap::<String, String>::new();

        for (idx, (formal_param_name, formal_param_type_info)) in decl.params.iter().enumerate() {
            let actual_param_type_info = &actual_param_types[idx];

            if actual_param_type_info.kind_discriminant() != formal_param_type_info.kind_discriminant() {
                return Err(TypeCheckError::new(
                    &token,
                    format!("incompatible param types for function {}, field {}", function_call.name, formal_param_name),
                ));
            }

            let is_generic = formal_param_type_info.is_generic;
            let fn_name = function_call.name.clone();

            let mut check_type_str = |formal_type_str: &String, actual_type_str: &String| {
                if is_generic {
                    match generic_type_str_to_actual_type_str.entry(formal_type_str.to_string()) {
                        Entry::Occupied(occupied_entry) => {
                            let found_type_str = occupied_entry.get();
                            if found_type_str != actual_type_str {
                                return Err(TypeCheckError::new(
                                    &token,
                                    format!("Ambigous generic type {formal_type_str}, field {formal_param_name}: {found_type_str}, {actual_type_str}"),
                                ));
                            }
                        }
                        Entry::Vacant(vacant_entry) => {
                            vacant_entry.insert(actual_type_str.clone());
                        }
                    }
                } else {
                    if formal_type_str != actual_type_str {
                        return Err(TypeCheckError::new(
                            &token,
                            format!("incompatible param types for field {formal_param_name}: expected {formal_type_str}, found {actual_type_str}"),
                        ));
                    }
                }

                Ok(())
            };

            match &formal_param_type_info.kind {
                TypeInfoKind::Scalar(scalar_type_str) => {
                    check_type_str(scalar_type_str, actual_param_type_info.get_scalar_type_str())?;
                }
                TypeInfoKind::Array(array_type_str) => {
                    check_type_str(array_type_str, actual_param_type_info.get_array_type_str())?;
                }
                TypeInfoKind::HashMap(formal_key_type_str, formal_value_type_str) => {
                    let actual_type_str_list = actual_param_type_info.get_hashmap_type_str_list();
                    check_type_str(formal_key_type_str, &actual_type_str_list[0])?;
                    check_type_str(formal_value_type_str, &actual_type_str_list[1])?;
                }
            }
        }

        for generic_type_str in &decl.generic_params {
            if !generic_type_str_to_actual_type_str.contains_key(generic_type_str) {
                return Err(TypeCheckError::new(&token, format!("can't infer generic param {generic_type_str}")));
            }
        }

        if let Some(return_type) = &decl.return_type {
            if !return_type.is_generic {
                return Ok(return_type.clone());
            }

            match &return_type.kind {
                TypeInfoKind::Scalar(generic_type_str) => {
                    let actual_type_str = generic_type_str_to_actual_type_str.get(generic_type_str).unwrap();
                    return Ok(TypeInfo::new_scalar(actual_type_str));
                }
                TypeInfoKind::Array(generic_type_str) => {
                    let actual_type_str = generic_type_str_to_actual_type_str.get(generic_type_str).unwrap();
                    return Ok(TypeInfo::new_array(actual_type_str));
                }
                TypeInfoKind::HashMap(key_generic_type_str, value_generic_type_str) => {
                    todo!();
                }
            }
        }

        // return Err(TypeCheckError::new(
        //     &expression.token,
        //     format!("can't figure return type of '{}' function call, please specify return type explicitly", function_call.name),
        // ));
        Ok(TypeInfo::new_scalar("none"))
    }

    fn fill_binary_op_type_info(&mut self, expression: &mut Expression) -> TypeCheckResult<TypeInfo> {
        match &mut *expression.kind {
            ExpressionKind::BinaryOperation { operator, left, right } => {
                let left_side_type_info = self.fill_expression_type(left)?;

                if operator == "." {
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

                let right_side_type_info = self.fill_expression_type(right)?;

                if operator == ".." {
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
                        if ["<", "<=", ">", ">=", "=", "<>", "and", "or"].contains(&operator.as_str()) {
                            return Ok(TypeInfo::new_scalar("bool"));
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
            _ => panic!(),
        }
    }

    fn fill_expression_type(&mut self, expression: &mut Expression) -> TypeCheckResult<TypeInfo> {
        let type_info = match &mut *expression.kind {
            ExpressionKind::IntegerLiteral(..) => TypeInfo::new_scalar("int"),
            ExpressionKind::BooleanLiteral(..) => TypeInfo::new_scalar("bool"),
            ExpressionKind::RealLiteral(..) => TypeInfo::new_scalar("real"),
            ExpressionKind::StringLiteral(..) => TypeInfo::new_scalar("str"),
            ExpressionKind::Identifier(identifier_name) => {
                //
                match identifier_name.as_str() {
                    "it" => {
                        if self.ctx.iterator_type_list.is_empty() {
                            return Err(TypeCheckError::new(&expression.token, "Keyword 'it' is reserved and can be used only in iterations"));
                        }
                        let type_info = self.ctx.iterator_type_list.last().unwrap();
                        if !type_info.is_array() {
                            return Err(TypeCheckError::new(&expression.token, format!("iteratable must be of array type")));
                        }
                        TypeInfo::new_scalar(type_info.get_array_type_str())
                    }
                    "idx" => TypeInfo::new_scalar("int"),
                    else_ => {
                        //
                        match self.get_function_param_or_var_type(&identifier_name) {
                            Some(type_info) => type_info.clone(),
                            None => {
                                return Err(TypeCheckError::new(&expression.token, "undeclared variablem"));
                            }
                        }
                    }
                }
            }
            ExpressionKind::UnaryOperation { operator, expr } => {
                let type_info = self.fill_expression_type(expr)?;
                if operator == "not" {
                    TypeInfo::new_scalar("bool")
                } else {
                    type_info
                }
            },
            ExpressionKind::BinaryOperation { operator, left, right } => self.fill_binary_op_type_info(expression)?,
            ExpressionKind::Group(group_expressions) => {
                if group_expressions.is_empty() {
                    return Err(TypeCheckError::new(&expression.token, "empty group"));
                }
                let mut type_info = None;
                for expression in group_expressions.iter_mut() {
                    type_info = Some(self.fill_expression_type(expression)?);
                }
                type_info.unwrap()
            }
            ExpressionKind::ArrayItemAccess { array_expression, access_expression } => {
                let array_type_info = self.fill_expression_type(array_expression)?;
                if !array_type_info.is_array() {
                    return Err(TypeCheckError::new(&expression.token, format!("not an array")));
                }
                let accessor_type_info = self.fill_expression_type(access_expression)?;
                if !accessor_type_info.is_scalar_type_str("int") {
                    return Err(TypeCheckError::new(&expression.token, format!("incorrect index type '{accessor_type_info}'")));
                }
                TypeInfo::new_scalar(array_type_info.get_array_type_str())
            }
            ExpressionKind::ArrayInitializer(identifier) => {
                let type_str = identifier;
                let type_info = TypeInfo::new_array(type_str.as_str());
                if is_builtin_type(&type_str) || self.ast_builder.records.contains_key(type_str) {
                    type_info
                } else {
                    return Err(TypeCheckError::new(&expression.token, format!("unknown type: '{type_str}'")));
                }
            }
            ExpressionKind::HashMapInitializer(key_type_str, value_type_str) => {
                if !is_builtin_type(key_type_str) && !self.ast_builder.records.contains_key(key_type_str) {
                    return Err(TypeCheckError::new(&expression.token, format!("unknown type: '{key_type_str}'")));
                }
                TypeInfo::new_hashmap(key_type_str.clone(), value_type_str.clone())
            }
            ExpressionKind::ObjectInitializer(record_name) => {
                //
                match self.ast_builder.records.get(record_name) {
                    Some(record) => TypeInfo::new_scalar(record.name.clone()),
                    None => return Err(TypeCheckError::new(&expression.token, format!("unknown type: '{record_name}'"))),
                }
            }
            ExpressionKind::FunctionCall(function_call) => self.fill_function_call_type_info(function_call, &expression.token)?,
            _ => panic!(), /* println!("{expression:?}"); */
        };

        expression.type_info = Some(type_info.clone());
        return Ok(type_info);
    }

    fn process_block_statements(&mut self, block: &mut Block) -> TypeCheckResult<()> {
        for statement in &mut block.statements {
            match statement {
                Statement::If(if_statement) => {
                    for (if_expression, block) in &mut if_statement.if_blocks {
                        let expression = self.fill_expression_type(if_expression)?;
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
                            let return_type = self.fill_expression_type(expression)?;
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
                    let type_info = self.fill_expression_type(iterable_expression)?;
                    iterable_expression.type_info = Some(type_info.clone());
                    self.ctx.iterator_type_list.push(type_info);
                    let block = self.process_block_statements(block)?;
                    self.ctx.iterator_type_list.pop();
                }
                Statement::VariableAssignment(variable_assignment) => {
                    let name = variable_assignment.name.clone();
                    let var_type = self.fill_expression_type(&mut variable_assignment.rvalue_expression)?;
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
                    let left_side_type_info = self.fill_expression_type(lvalue)?;
                    let right_side_type_info = self.fill_expression_type(rvalue)?;
                    if !can_lift_type_from_to(&right_side_type_info, &left_side_type_info) {
                        return Err(TypeCheckError::new(
                            token,
                            format!("incompatible types, can't assign to '{left_side_type_info}' from '{right_side_type_info}'"),
                        ));
                    }
                }
                // Statement::FunctionCall(function_call) => {
                //     let mut new_params = vec![];
                //     for param in &function_call.params {
                //         let type_info = self.get_expression_type(param)?;
                //         let mut new_param = param.clone();
                //         new_param.type_info = Some(type_info);
                //         new_params.push(new_param);
                //     }
                //     function_call.params = new_params;
                // }
                Statement::Expression(expr) => {
                    let type_info = self.fill_expression_type(expr)?;
                }
                Statement::Break() => {}
                Statement::Continue() => {}
                Statement::Comment(_) => {}
                _ => panic!(),
            }
        }

        Ok(())
    }
}
