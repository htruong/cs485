/**
  * Eratosthenes Sieve Thread
  * Written by Huan Truong <hnt7438@truman.edu>
  * This implements a worker thread
  * that diligently marks numbers as non-primes
  */

package EratosthenesSieve;

import java.lang.Thread;

///////////////////////////////////////////////////////////////////////////////

public class TruongSieveThread extends Thread {
  TruongSieve s;
  int bound;

  /** Construct and initialize a sieve thread.
    * @param name The name given to this thread
    * @param sieve The shared sieve
    */
  public TruongSieveThread (String name, TruongSieve sieve, int upperBound) {
    setName (name);
    s = sieve;
    bound = upperBound;
  }

  /**
    * Begin execution of a Sieve Thread.
    * Marks all non-prime numbers
    */
  public void run() {
    int workingUnmarked = s.getNextUnmarked();

    // Do it while we have at least one unmarked number to be processed
    while((workingUnmarked*workingUnmarked <= bound) && (workingUnmarked!=-1)) {
      // System.out.println( getName() +  
      //            " is working on prime number " + workingUnmarked );
      int i = 2;
      while (i * workingUnmarked <= bound) {
        s.mark(i * workingUnmarked);
        i++;
      }
      workingUnmarked = s.getNextUnmarked();
    }
  }
}

