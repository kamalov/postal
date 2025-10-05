use indexmap::{IndexSet};

pub fn intersects<T>(a: &IndexSet<T>, b: &Vec<T>) -> bool 
where T: PartialEq {
    a.iter().any(|s| b.contains(s))
}

pub fn readln() {
    let mut guess = String::new();
    std::io::stdin().read_line(&mut guess).unwrap();
}
