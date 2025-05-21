import java.util.TreeSet;
import java.io.*;

public class Test {
    public static void main(String[] args) throws IOException {
        var set = new TreeSet<Integer>();
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        int n = Integer.parseInt(br.readLine());
        var start = System.currentTimeMillis();
        for (int i = 0; i < n; i++) {
            set.add(Integer.parseInt(br.readLine()));
        }
        var stop = System.currentTimeMillis();
        System.out.println("insertion took: " + (double)(stop-start)/1000 + " s");
        start = System.currentTimeMillis();
        int sum = 0; 
        for (var v : set) {
            sum += v;
        }
        stop = System.currentTimeMillis();
        System.out.println("iteration took: " + (double)(stop-start)/1000 + "s, " + sum);
    }
}
