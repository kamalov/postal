use std::collections::{HashMap, HashSet};
use std::fmt::{format, Write};
use indexmap::IndexMap;
use crate::type_info::*;
use crate::stage02_ast_builder::*;
use crate::stage03_type_checker::*;

const OPERATORS_MAP: [(&str, &str); 11] = [
    (".", "->"),
    ("+", " + "),
    ("-", " - "),
    ("=", " == "),
    (">", " > "),
    (">=", " >= "),
    ("<", " < "),
    ("<=", " <= "),
    ("and", " && "),
    ("or", " || "),
    ("<>", " != "),
];

const PADDING: &str = "    ";

struct CurrentFunctionContext {
    function_node: Function,
    iterators: Vec<(String, String)>,
    iterators_count: u32,
}

pub struct CodeGenerator {
    ast_builder: AstBuilder,
    operators_map: HashMap<String, String>,
    current_function_context: Option<CurrentFunctionContext>,
}

impl CodeGenerator {
    pub fn new(ast_builder: AstBuilder) -> CodeGenerator {
        let mut operators_map = HashMap::new();
        for (op_from, op_to) in OPERATORS_MAP {
            operators_map.insert(op_from.to_string(), op_to.to_string());
        }

        CodeGenerator {
            ast_builder,
            current_function_context: None,
            operators_map,
        }
    }

    pub fn generate_code(&mut self) -> String {
        let mut r = String::new();
        for n in &self.ast_builder.root_nodes.clone() {
            match n {
                RootNode::Record(record_node) => {
                    let code = self.generate_record_code(record_node, "");
                    r.push_str(code.as_str());
                }
                RootNode::Function(function_node) => {
                    let code = self.generate_function_code(function_node, "");
                    r.push_str(code.as_str());
                }
                RootNode::Comment(line_comment) => {
                    let code = self.generate_comment_code(line_comment, "");
                    r.push_str(code.as_str());
                }
                _ => (),
            }
        }

        r
    }

    fn generate_record_code(&mut self, record_node: &Record, padding: &str) -> String {
        let mut result_str = String::new();

        writeln!(&mut result_str, "{padding}struct {} {{", record_node.name);

        for (field_name, type_info) in &record_node.fields {
            let type_declaration_str = type_info.to_declaration_string();
            writeln!(&mut result_str, "{padding}{PADDING}{type_declaration_str} {field_name};");
        }

        let op = self.generate_record_equality_operator(record_node, format!("{padding}{PADDING}").as_str());

        write!(&mut result_str, "{op}");
        writeln!(&mut result_str, "}};");
        writeln!(&mut result_str, "");

        result_str
    }

    fn generate_record_equality_operator(&mut self, record_node: &Record, padding: &str) -> String {
        let mut result_str = String::new();
        writeln!(&mut result_str, "{padding}friend bool operator==(const {0}& l, const {0}& r) {{", record_node.name);
        let mut parts = vec![];
        for (field_name, type_info) in &record_node.fields {
            if type_info.is_generic {
                continue;
            }
            
            if type_info.is_scalar() {
                let type_str = type_info.get_scalar_type_str();
                if !is_custom_type(&type_str) {
                    parts.push(format!("(l.{0} == r.{0})", field_name));
                }
            }
        }
        writeln!(&mut result_str, "{padding}{PADDING}return {};", parts.to_vec().join(" && "));
        writeln!(&mut result_str, "{padding}}}");
        result_str
    }

    fn generate_function_code(&mut self, function_node: &Function, padding: &str) -> String {
        self.current_function_context = Some(CurrentFunctionContext {
            function_node: function_node.clone(),
            iterators: vec![],
            iterators_count: 0,
        });

        let fn_generic_params_code = self.generate_function_declaration_generic_params_code(&function_node.declaration.generic_params);
        let fn_params_code = self.generate_function_declaration_params_code(&function_node.declaration);
        let fn_body_code = self.generate_block_code(&function_node.body, padding);

        let mut r = String::new();
        let return_type_str = match &function_node.declaration.return_type {
            Some(info) => info.to_declaration_string(),
            None => "void".to_string(),
        };

        if function_node.is_external {
            writeln!(
                &mut r,
                "{padding}{fn_generic_params_code}{return_type_str} {name}({fn_params_code});",
                name = function_node.declaration.name
            );
        } else {
            writeln!(
                &mut r,
                "{padding}{fn_generic_params_code}{return_type_str} {name}({fn_params_code}) {{",
                name = function_node.declaration.name
            );

            for (name, type_info) in &function_node.vars {
                let type_declaration_str = type_info.to_declaration_string();
                // if is_custom_type(&type_info.type_str) {
                //     let type_initializer_str = type_info_to_initializer_str(type_info);
                //     writeln!(&mut r, "    {type_declaration_str} {name} = {type_initializer_str};");
                // } else {
                    writeln!(&mut r, "{padding}{PADDING}{type_declaration_str} {name};");
                // }
            }

            write!(&mut r, "{fn_body_code}");
            writeln!(&mut r, "}}");
            writeln!(&mut r, "");
        }

        self.current_function_context = None;

        r
    }

    fn generate_function_declaration_generic_params_code(&self, generic_params: &Vec<String>) -> String {
        if generic_params.is_empty() {
            return String::new();
        }

        let mut parts = vec![];
        for name in generic_params {
            let s = format!("typename {name}");
            parts.push(s);
        }

        format!("template <{}>", parts.join(", "))
    }

    fn generate_function_declaration_params_code(&self, declaration: &FnDeclaration) -> String {
        let mut parts = vec![];
        for (name, type_info) in &declaration.params {
            let type_declaration_str = type_info.to_declaration_string();
            let s = format!("{type_declaration_str} {name}");
            parts.push(s);
        }

        parts.join(", ")
    }

    fn generate_block_code(&mut self, b: &Block, padding: &str) -> String {
        let mut r = String::new();

        let mut s = vec![];
        for statement in &b.statements {
            let padding = format!("{}{}", padding, PADDING);
            let statement_code = self.generate_statement_code(statement, padding.as_str());
            s.push(statement_code);
        }
        let s = s.join("");
        write!(&mut r, "{s}");

        r
    }

    fn generate_statement_code(&mut self, statement: &Statement, padding: &str) -> String {
        let mut r = String::new();

        match statement {
            Statement::If(st) => {
                let s = self.generate_if_statement_code(st, padding);
                r.push_str(s.as_str());
            }
            Statement::Loop(st) => {
                let s = self.generate_loop_statement_code(st, padding);
                r.push_str(s.as_str());
            }
            Statement::Iteration(iteration_statement) => {
                let s = self.generate_iteration_code(iteration_statement, padding);
                r.push_str(s.as_str());
            }
            Statement::For(for_statement) => {
                let s = self.generate_for_code(for_statement, padding);
                r.push_str(s.as_str());
            }
            Statement::FunctionCall(st) => {
                let s = self.generate_function_call_code(st);
                let s = format!("{}{};\n", padding, s);
                r.push_str(s.as_str());
            }
            Statement::VariableAssignment(v) => {
                let s = self.generate_variable_assignment_code(v, padding);
                r.push_str(s.as_str());
            }
            Statement::Break() => {
                writeln!(&mut r, "{padding}break;");
            }
            Statement::Continue() => {
                writeln!(&mut r, "{padding}continue;");
            }
            Statement::Comment(line_comment) => {
                let s = self.generate_comment_code(line_comment, padding);
                r.push_str(s.as_str());
            }
            Statement::Return(expression_option) => match expression_option {
                None => {
                    writeln!(&mut r, "{padding}return;");
                }
                Some(expression) => {
                    writeln!(&mut r, "{padding}return {expr};", expr = self.generate_expression_code(&expression));
                }
            },
            Statement::VariableDeclaration(_) => {}
            Statement::Assignment(assignment_node) => {
                let s = self.generate_assignment_code(assignment_node, padding);
                r.push_str(s.as_str());
            }
            _ => {
                println!("{:?}", statement);
                panic!();
            }
        }

        r
    }

    fn generate_if_statement_code(&mut self, f: &IfStatement, padding: &str) -> String {
        let mut r = String::new();

        for i in 0..f.if_blocks.len() {
            let (condition, block) = &f.if_blocks[i];
            let expr = self.generate_expression_code(&condition);
            let block = self.generate_block_code(&block, padding);

            writeln!(&mut r, "");
            if i == 0 {
                writeln!(&mut r, "{}if ({}) {{", padding, expr);
            } else {
                writeln!(&mut r, "{}else if ({}) {{", padding, expr);
            }
            write!(&mut r, "{}", block);
            write!(&mut r, "{}}}", padding);
        }

        if let Some(block) = &f.else_block {
            let block_str = self.generate_block_code(&block, padding);
            writeln!(&mut r, "");
            writeln!(&mut r, "{}else {{", padding);
            write!(&mut r, "{}", block_str);
            write!(&mut r, "{}}}", padding);
        }

        writeln!(&mut r, ";");

        r
    }

    fn generate_loop_statement_code(&mut self, f: &LoopStatement, padding: &str) -> String {
        let mut r = String::new();

        let block = self.generate_block_code(&f.block, padding);
        writeln!(&mut r, "");
        writeln!(&mut r, "{}for (;;) {{", padding);
        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}}}", padding);

        r
    }

    fn generate_iteration_code(&mut self, iteration_node: &IterationStatement, padding: &str) -> String {
        let mut r = String::new();

        let mut ctx = self.current_function_context.as_mut().unwrap();
        let index = ctx.iterators_count;
        ctx.iterators_count += 1;
        let iteratable_type_info = match ctx.function_node.vars.get(&iteration_node.iterable_name) {
            Some(type_info) => Some(type_info),
            None => match ctx.function_node.declaration.params.get(&iteration_node.iterable_name) {
                Some(type_info) => Some(type_info),
                None => None,
            },
        };

        let iteratable_type_info = iteratable_type_info.unwrap();

        let iteratable_name = iteration_node.iterable_name.clone();
        let it_name = format!("{}__it{}", iteratable_name, index);
        let it_index_name = format!("{}__idx", it_name);
        let it_type_name = iteratable_type_info.get_array_type_str().clone();
        ctx.iterators.push((it_name.clone(), it_type_name.clone()));

        let block = self.generate_block_code(&iteration_node.block, padding);
        writeln!(&mut r, "");
        writeln!(&mut r, "{0}for (int {1} = 0; {1} < {2}->size(); {1}++) {{", padding, it_index_name, iteratable_name);
        let type_info = TypeInfo::new_scalar(it_type_name);
        let type_declaration_str = type_info.to_declaration_string();
        writeln!(&mut r, "{padding}{PADDING}{type_declaration_str} {it_name} = {iteratable_name}->at({it_index_name});");

        write!(&mut r, "{}", block);
        writeln!(&mut r, "{}}}", padding);

        self.current_function_context.as_mut().unwrap().iterators.pop();

        r
    }

    fn generate_for_code(&mut self, for_statement: &ForStatement, padding: &str) -> String {
        let mut r = String::new();

        let index = self.current_function_context.as_ref().unwrap().iterators_count;
        let iteratable_type_info = for_statement.iterable_expression.type_info.clone().unwrap();
        let it_expression_var_name = format!("__expr{index}");
        let iteratable_expression_code = self.generate_expression_code(&for_statement.iterable_expression);
        writeln!(&mut r, "");
        writeln!(&mut r, "{padding}auto {it_expression_var_name} = {iteratable_expression_code};");

        let mut ctx = self.current_function_context.as_mut().unwrap();
        let index = ctx.iterators_count;
        ctx.iterators_count += 1;

        let iteratable_name = match &*for_statement.iterable_expression.kind {
            ExpressionKind::Identifier(identifier) => identifier.clone(),
            _ => "expr".to_string(),
        };

        let it_name = format!("{}__it{}", iteratable_name, index);
        let it_index_name = format!("{}__idx", it_name);
        let it_type_name = iteratable_type_info.get_array_type_str();
        ctx.iterators.push((it_name.clone(), it_type_name.clone()));
        let block = self.generate_block_code(&for_statement.block, padding);
        writeln!(&mut r, "{padding}for (int {0} = 0; {0} < {it_expression_var_name}->size(); {0}++) {{", it_index_name);
        let type_info = TypeInfo::new_scalar(it_type_name);
        let type_declaration_str = type_info.to_declaration_string();
        writeln!(&mut r, "{padding}{PADDING}{type_declaration_str} {it_name} = (*{it_expression_var_name})[{it_index_name}];");
        write!(&mut r, "{}", block);
        writeln!(&mut r, "{padding}}}");

        self.current_function_context.as_mut().unwrap().iterators.pop();

        r
    }

    fn generate_function_call_code(&mut self, f: &FunctionCall) -> String {
        let mut r = String::new();

        let mut param_names = vec![];
        for expression in &f.params {
            let s = self.generate_expression_code(&expression);
            param_names.push(s);
        }
        let p = param_names.join(", ");
        let function_name = f.name.as_str();
        match function_name {
            "log" => {
                if f.params.is_empty() {
                    write!(&mut r, "printf(\"\\n\")");
                } else {
                    let mut format_parts = vec![];
                    let mut names = vec![];
                    for (i, expression) in f.params.iter().enumerate() {
                        let param_name = param_names[i].clone();
                        match &expression.type_info {
                            Some(type_info) => {
                                //
                                match type_info.get_scalar_type_str().as_str() {
                                    "int" => {
                                        format_parts.push("%lld");
                                        names.push(format!("static_cast<long long>({})", param_name));
                                    }
                                    "real" => {
                                        format_parts.push("%f");
                                        names.push(param_name.clone());
                                    }
                                    "str" => {
                                        format_parts.push("%s");
                                        names.push(format!("({}).c_str()", param_name.clone()));
                                    }
                                    else_ => {
                                        format_parts.push("%d");
                                        //names.push(format!("(void *)&{param_name}"));
                                        names.push(format!("{param_name}"));
                                    }
                                }
                            }
                            None => {
                                format_parts.push("%d");
                                //names.push(format!("(void *)&{param_name}"));
                                names.push(format!("{param_name}"));
                            }
                        }
                    }
                    write!(&mut r, "printf(\"{}\\n\", {})", format_parts.join(" "), names.join(", "));
                }
            }

            _ => {
                write!(&mut r, "{}({})", function_name, p);
            }
        }

        r
    }

    fn generate_group_code(&mut self, expressions: &Vec<Expression>, padding: &str) -> String {
        let mut s = String::new();

        let mut expression_code_list = vec![];
        for expression in expressions {
            let s = self.generate_expression_code(&expression);
            expression_code_list.push(s);
        }
        let code = expression_code_list.join(", ");
        write!(&mut s, "{}({})", padding, code);

        s
    }

    fn generate_comment_code(&mut self, line_comment: &String, padding: impl Into<String>) -> String {
        let mut s = String::new();
        writeln!(&mut s, "{}//{line_comment}", padding.into());
        s
    }

    fn generate_array_item_access_code(&mut self, array_expression: &Expression, index_expression: &Expression) -> String {
        let mut s = String::new();
        let array_expression_code = self.generate_expression_code(array_expression);
        let index_expression_code = self.generate_expression_code(index_expression);
        write!(&mut s, "(*{array_expression_code})[{index_expression_code}]");
        s
    }

    fn generate_array_initializer_code(&mut self, type_identifier: &str) -> String {
        let mut result_str = String::new();

        let type_info = TypeInfo::new_array(type_identifier);
        let type_initializer_str = type_info.to_initializer_string();
        write!(&mut result_str, "{type_initializer_str}");
        result_str
    }

    fn generate_hashmap_initializer_code(&mut self, key_type_str: &str, value_type_str: &str) -> String {
        let mut result_str = String::new();

        let type_info = TypeInfo::new_hashmap(key_type_str, value_type_str);
        let type_initializer_str = type_info.to_initializer_string();
        write!(&mut result_str, "{type_initializer_str}");
        result_str
    }

    fn generate_variable_assignment_code(&mut self, v: &VariableAssignment, padding: &str) -> String {
        let mut r = String::new();

        let s = self.generate_expression_code(&v.rvalue_expression);
        writeln!(&mut r, "{}{} = {};", padding, v.name, s);

        r
    }

    fn generate_assignment_code(&mut self, assignment_node: &Assignment, padding: &str) -> String {
        let mut result_str = String::new();

        let left = self.generate_expression_code(&assignment_node.lvalue);
        let right = self.generate_expression_code(&assignment_node.rvalue);
        writeln!(&mut result_str, "{padding}{left} = {right};");

        result_str
    }

    fn generate_object_initializer_code(&mut self, record_name: &str) -> String {
        let mut result_str = String::new();

        write!(&mut result_str, "new {record_name}()");
        result_str
    }

    fn generate_expression_code(&mut self, expression: &Expression) -> String {
        let mut r = String::new();

        match &*expression.kind {
            ExpressionKind::Identifier(identifier_name) => {
                if identifier_name == "it" || identifier_name == "idx" {
                    match &self.current_function_context {
                        None => {
                            panic!("it or idx is reserved and allowed only inside iterators inside functions");
                        }
                        Some(current_function_context) => {
                            let iterators = &current_function_context.iterators;
                            if iterators.is_empty() {
                                panic!("it or idx is reserved and allowed only inside iterators");
                            }

                            let (it_name, it_type_str) = iterators.last().unwrap();
                            match identifier_name.as_str() {
                                "it" => {
                                    write!(&mut r, "{}", it_name.clone());
                                }
                                "idx" => {
                                    write!(&mut r, "{}", it_name.clone() + "__idx");
                                }
                                _ => panic!(),
                            }
                        }
                    }
                } else {
                    write!(&mut r, "{}", identifier_name);
                }
            }
            ExpressionKind::Operator(operator) => {
                write!(&mut r, " {} ", operator);
            }
            ExpressionKind::IntegerLiteral(literal) => {
                write!(&mut r, "{}ll", literal);
            }
            ExpressionKind::RealLiteral(literal) => {
                write!(&mut r, "{}", literal);
            }
            ExpressionKind::StringLiteral(literal) => {
                write!(&mut r, "\"{}\"s", literal);
            }
            ExpressionKind::BinaryOperation { operation, left, right } => {
                let left = self.generate_expression_code(&left);
                let right = self.generate_expression_code(&right);
                let op_str = self.operators_map.get(operation).cloned().unwrap_or(operation.clone());
                match operation.as_str() {
                    ".." => {
                        write!(&mut r, "create_range({left}, {right})");
                    }
                    _ => {
                        write!(&mut r, "{left}{op_str}{right}");
                    }
                }
            }
            ExpressionKind::Group(group_expressions) => {
                let code = self.generate_group_code(group_expressions, "");
                write!(&mut r, "{}", code);
            }
            ExpressionKind::FunctionCall(function_call) => {
                let code = self.generate_function_call_code(&function_call);
                write!(&mut r, "{}", code);
            }
            ExpressionKind::ArrayItemAccess { array_expression, access_expression } => {
                let code = self.generate_array_item_access_code(&array_expression, &access_expression);
                write!(&mut r, "{}", code);
            }
            ExpressionKind::ArrayInitializer(type_identifier) => {
                let code = self.generate_array_initializer_code(&type_identifier);
                write!(&mut r, "{}", code);
            }
            ExpressionKind::HashMapInitializer(k, v) => {
                let code = self.generate_hashmap_initializer_code(&k, &v);
                write!(&mut r, "{}", code);
            }
            ExpressionKind::ObjectInitializer(record_name) => {
                let code = self.generate_object_initializer_code(&record_name);
                write!(&mut r, "{}", code);
            }
            panic_ => {
                println!("{:?}", panic_);
                panic!();
            }
        }

        r
    }
}
