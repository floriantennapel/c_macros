use std::io;
use std::collections::BTreeSet;
use std::time::Instant;

fn main() {
    let mut in_iter = io::stdin().lines();
    let _ = in_iter.next().unwrap().unwrap().parse::<usize>().unwrap();

    let mut now = Instant::now();
    let mut tree: BTreeSet<i32> = BTreeSet::new();
    for line in in_iter {
        tree.insert(line.unwrap().parse::<i32>().unwrap());
    }
    println!("insertion took {} s", now.elapsed().as_millis() as f64 / 1000.);

    now = Instant::now();
    let mut sum: i32 = 0;
    for d in tree.iter() {
        sum = sum.wrapping_add(*d);
    }
    println!("iteration took {} s, {sum}", now.elapsed().as_millis() as f64 / 1000.);
}
