use std::io;
use std::collections::BTreeSet;
use std::time::Instant;

fn main() {
    let mut in_iter = io::stdin().lines();
    let _ = in_iter.next().unwrap().unwrap().parse::<usize>().unwrap();
    let input: Vec<i32> = in_iter
        .map(|l| l.unwrap().parse::<i32>().unwrap())
        .collect();

    let mut now = Instant::now();
    let mut tree: BTreeSet<i32> = BTreeSet::new();
    for num in &input {
        tree.insert(*num);
    }
    println!("insertion took {} s", now.elapsed().as_millis() as f64 / 1000.);

    let mut sum: i32 = 0;
    now = Instant::now();
    for num in &input {
        if tree.contains(&num) {
            sum = sum.wrapping_add(*num);
        }
    }
    println!("queries took {} s", now.elapsed().as_millis() as f64 / 1000.);

    now = Instant::now();
    for d in tree.iter() {
        sum = sum.wrapping_add(*d);
    }
    println!("iteration took {} s, {sum}", now.elapsed().as_millis() as f64 / 1000.);
}
