import java.util.*;
import java.io.*;

public class Test {
    public static void main(String[] args) throws IOException {
        var set = new HashSet<Integer>();
        var input = new ArrayList<Integer>();
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        int n = Integer.parseInt(br.readLine());
        for (int i = 0; i < n; i++) {
            input.add(Integer.parseInt(br.readLine()));
        }

        var start = System.currentTimeMillis();
        for (var num : input)
            set.add(num);
        var stop = System.currentTimeMillis();
        System.out.println("insertion took: " + (double)(stop-start)/1000 + " s");

        int sum = 0; 
        start = System.currentTimeMillis();
        for (var num : input) {
            if (set.contains(num))    
                sum += num;
        }
        stop = System.currentTimeMillis();
        System.out.println("queries took: " + (double)(stop-start)/1000 + " s");

        start = System.currentTimeMillis();
        for (var v : set) {
            sum += v;
        }
        stop = System.currentTimeMillis();
        System.out.println("iteration took: " + (double)(stop-start)/1000 + "s, " + sum);
    }
}
