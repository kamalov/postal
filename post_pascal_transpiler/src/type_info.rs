use std::vec;
use std::collections::hash_map::Entry;
use std::collections::HashMap;
use indexmap::IndexMap;
use crate::utils::*;

pub const BUILTIN_TYPES: [&str; 3] = ["int", "real", "str"];

pub const BUILTIN_FUNCTIONS: [&str; 1] = ["log"];

pub fn is_builtin_function(fn_name: &str) -> bool {
    BUILTIN_FUNCTIONS.contains(&fn_name)
}

pub fn is_builtin_type(type_str: &str) -> bool {
    BUILTIN_TYPES.contains(&type_str)
}

pub fn is_custom_type(type_str: &str) -> bool {
    !BUILTIN_TYPES.contains(&type_str)
}

pub fn get_common_type(a: &TypeInfo, b: &TypeInfo) -> Result<TypeInfo, ()> {
    if a == b {
        return Ok(a.clone());
    }

    if let TypeInfoKind::Scalar(a_type_str) = &a.kind {
        if let TypeInfoKind::Scalar(b_type_str) = &b.kind {
            if a_type_str == "real" && b_type_str == "int" || a_type_str == "int" && b_type_str == "real" {
                return Ok(TypeInfo::new_scalar("real"));
            }
        }
    }
    
    return Err(());
}

pub fn can_lift_type_from_to(from: &TypeInfo, to: &TypeInfo) -> bool {
    if from == to {
        return true;
    }

    if let TypeInfoKind::Scalar(from_type_str) = &from.kind {
        if let TypeInfoKind::Scalar(to_type_str) = &to.kind {
            if from_type_str == "int" && to_type_str == "real" {
                return true;
            }
        }
    }
    
    return false;
}

#[derive(Debug, Clone, PartialEq)]
pub enum TypeInfoKind {
    HashMap(String, String),
    Array(String),
    Scalar(String),
}

#[derive(Debug, Clone, PartialEq)]
pub struct TypeInfo {
    pub is_generic: bool,
    pub kind: TypeInfoKind,
}

impl TypeInfo {
    pub fn new(kind: TypeInfoKind) -> TypeInfo {
        TypeInfo { kind, is_generic: false }
    }

    pub fn new_scalar(s: impl Into<String>) -> TypeInfo {
        TypeInfo::new(TypeInfoKind::Scalar(s.into()))
    }

    pub fn new_array(s: impl Into<String>) -> TypeInfo {
        TypeInfo::new(TypeInfoKind::Array(s.into()))
    }

    pub fn new_hashmap(k: impl Into<String>, v: impl Into<String>) -> TypeInfo {
        TypeInfo::new(TypeInfoKind::HashMap(k.into(), v.into()))
    }

    pub fn get_type_str_list(&self) -> Vec<String> {
        return match &self.kind {
            TypeInfoKind::HashMap(t1, t2) => vec![t1.clone(), t2.clone()],
            TypeInfoKind::Array(t) => vec![t.clone()],
            TypeInfoKind::Scalar(t) => vec![t.clone()],
        }
    }

    pub fn is_scalar(&self) -> bool {
        if let TypeInfoKind::Scalar(_) = self.kind {
            true
        } else {
            false
        }
    }

    pub fn is_scalar_type_str(&self, s: impl Into<String>) -> bool {
        if let TypeInfoKind::Scalar(v) = &self.kind {
            *v == s.into()
        } else {
            false
        }
    }

    pub fn get_scalar_type_str(&self) -> &String {
        match &self.kind {
            TypeInfoKind::Scalar(t) => t,
            _ => panic!(),
        }
    }

    pub fn is_array(&self) -> bool {
        if let TypeInfoKind::Array(_) = self.kind {
            true
        } else {
            false
        }
    }

    pub fn get_array_type_str(&self) -> &String {
        match &self.kind {
            TypeInfoKind::Array(t) => t,
            _ => panic!(),
        }
    }

    pub fn to_cpp_type_string(&self) -> String {
        fn type_str_to_cpp_type_str(s: &String) -> String {
            match s.as_str() {
                "" => "void".to_string(),
                "int" => "long long".to_string(),
                "real" => "double".to_string(),
                "str" => "std::string".to_string(),
                _ => s.clone()
            }
        }

        match &self.kind {
            TypeInfoKind::HashMap(key_type_str, value_type_str) => {
                let key_cpp_type_str = type_str_to_cpp_type_str(key_type_str);
                let value_cpp_type_str = type_str_to_cpp_type_str(value_type_str);
                if !self.is_generic && is_custom_type(key_cpp_type_str.as_str()) {
                    return format!("universal_hashmap<{key_cpp_type_str}*, {value_cpp_type_str}>");
                } else {
                    return format!("universal_hashmap<{key_cpp_type_str}, {value_cpp_type_str}>");
                }
            }
            TypeInfoKind::Array(type_str) => {
                let cpp_type_str = type_str_to_cpp_type_str(&type_str);
                if !self.is_generic && is_custom_type(type_str) {
                    return format!("std::vector<{cpp_type_str}*>");
                } else {
                    return format!("std::vector<{cpp_type_str}>");
                }
            }
            TypeInfoKind::Scalar(type_str) => {
                let cpp_type_str = type_str_to_cpp_type_str(&type_str);
                cpp_type_str
            }
        }
    }

    pub fn to_declaration_string(&self) -> String {
        let cpp_type_str = self.to_cpp_type_string();
        
        match &self.kind {
            TypeInfoKind::HashMap(_, _) => {
                return format!("{cpp_type_str}*");    
            }
            TypeInfoKind::Array(_) => {
                return format!("{cpp_type_str}*");    
            }
            TypeInfoKind::Scalar(type_str) => {
                if !self.is_generic && is_custom_type(type_str) {
                    return format!("{cpp_type_str}*");    
                } else {
                    return format!("{cpp_type_str}");    
                }
            }
        }

        cpp_type_str
    }

    pub fn to_initializer_string(&self) -> String {
        let cpp_type_str = self.to_cpp_type_string();
        match &self.kind {
            TypeInfoKind::HashMap(key_type_str, value_type_str) => {
                return format!("new {cpp_type_str}()",);
            }
            TypeInfoKind::Array(_) => {
                return format!("new {cpp_type_str}()",);
            }
            TypeInfoKind::Scalar(type_str) => {
                if is_custom_type(&type_str) {
                    return format!("new {cpp_type_str}()",);
                }
            }
        }

        cpp_type_str
    }
}
