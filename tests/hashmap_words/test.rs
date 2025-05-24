use std::io;
use std::collections::HashSet;
use std::time::Instant;

fn main() {
    let input: Vec<String> = io::stdin()
        .lines()
        .map(|l| l.unwrap())
        .collect();

    let mut now = Instant::now();
    let mut tree: HashSet<String> = HashSet::new();
    for s in &input {
        tree.insert(s.clone());
    }
    println!("insertion took {} s", now.elapsed().as_millis() as f64 / 1000.);

    let mut sum: i32 = 0;
    now = Instant::now();
    for s in &input {
        if tree.contains(s) {
            sum = sum.wrapping_add(s.as_bytes()[0] as i32);
        }
    }
    println!("queries took {} s", now.elapsed().as_millis() as f64 / 1000.);

    now = Instant::now();
    for s in tree.iter() {
        sum = sum.wrapping_add(s.as_bytes()[0] as i32);
    }
    println!("iteration took {} s, {sum}", now.elapsed().as_millis() as f64 / 1000.);
}
