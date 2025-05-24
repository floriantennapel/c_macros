import java.util.*;
import java.io.*;

public class Test {
    public static void main(String[] args) throws IOException {
        var set = new HashSet<String>();
        var input = new ArrayList<String>();
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

        String line;
        while ((line = br.readLine()) != null)
            input.add(line);

        var start = System.currentTimeMillis();
        for (var str : input)
            set.add(str);
        var stop = System.currentTimeMillis();
        System.out.println("insertion took: " + (double)(stop-start)/1000 + " s");

        int sum = 0; 
        start = System.currentTimeMillis();
        for (var s : input) {
            if (set.contains(s))    
                sum += s.charAt(0);
        }
        stop = System.currentTimeMillis();
        System.out.println("queries took: " + (double)(stop-start)/1000 + " s");

        start = System.currentTimeMillis();
        for (var s : set) {
            sum += s.charAt(0);
        }
        stop = System.currentTimeMillis();
        System.out.println("iteration took: " + (double)(stop-start)/1000 + "s, " + sum);
    }
}
