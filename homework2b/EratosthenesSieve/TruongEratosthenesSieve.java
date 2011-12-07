/**
  * Eratosthenes Sieve
  * Written by Huan Truong <hnt7438@truman.edu>
  *
  */

package EratosthenesSieve;

public class TruongEratosthenesSieve {
  /**
    * Eratosthenes Sieve Java Implementation
    *
    * @param args Command line arguments
    */

  public static void main (String [] args) {
    int upperBound = Integer.parseInt(args[0]);
    int numThreads = Integer.parseInt(args[1]);

    // Benchmark: If we see 3 params, then we don't want output,
    // instead we just want the total number of primes.
    boolean supress_output = (args.length == 3);

    // Create a new shared sieve
    TruongSieve s = new TruongSieve(upperBound);

    // Create worker threads
    TruongSieveThread [] sThread = new TruongSieveThread [numThreads];

    for (int i = 0; i < numThreads; i++) {
      try {
          sThread[i] = new TruongSieveThread ("Worker" + i, s, upperBound);
          sThread[i].start();
      } catch (Throwable e) {
        System.out.println ("Couldn't create worker " + i);
      }
    }

    // Wait for all worker threads to join
    for (int i = 0; i < numThreads; i++) {
      try {
        sThread[i].join();
      } catch (Throwable e) {
        System.out.println ("Couldn't join with worker " + i);
      }
    }

    // Prints out all unmarked numbers in the sieve
    if (!supress_output) {
      s.print();
    } else {
      System.out.println("pi(" + upperBound + ") = " + s.count());
    }

  }
}
