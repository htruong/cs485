/**
  * Eratosthenes Sieve
  * Written by Huan Truong <hnt7438@truman.edu>
  * This implements the sieve object that handles markings and stuff.
  */
package EratosthenesSieve;

///////////////////////////////////////////////////////////////////////////////

public class TruongSieve {
  boolean[] sieve;

  // The bound of the sieve
  int upperBound;

  // The max number that we know that we have checked if it's marked or not
  int maxChecked;

  /** Constructs and initializes a sieve
    * @param name The name given to this sieve, not particularly helpful
    * @param max How big the sieve is
    */
  public TruongSieve (int max) {
    // Just a little bit wasteful to make our life easier and to prevent bugs.
    // We are creating an array that goes from 0..[max+1]

    upperBound = max + 1;

    sieve = new boolean[upperBound];

    // Unmarks everything
    for (int i = 2; i < upperBound; i++) sieve[i] = true;
    maxChecked = 1;
  }

  /**
    * Marks element as non-prime.
    * @param i The element to mark
    */
  public void mark(int i) {
    if (sieve[i]) {
      // We don't need the marking process to be sync'ed
      // As long as we know that we'll mark the entry sooner or later.
      //synchronized (this) {
        sieve[i] = false;
      //}
    }
  }

  /**
    * Checks if an element is marked
    * @param i The element to check
    */
  public boolean isMarked(int i) {
    return sieve[i];
  }

  /**
    * Returns the next unmarked number in the sieve
    */
  public int getNextUnmarked() {
    // The whole block needs to be sync'ed to prevent two threads
    // getting the same next unmarked number.
    // This might return a number that is not a prime but not yet marked
    // Say the thread that processes 2 hasn't got to the point to mark 8,
    // this is totally fine although we'll waste the thread that
    // works on multiplies of 8 here...
    synchronized (this) {
      int i = maxChecked + 1;
      while (i < upperBound) {
        maxChecked = i;
        //System.out.println("setting maxChecked =" + maxChecked );
        if (sieve[i]) return i;
        i++;
      }
      return -1;
    }
  }


  /**
    * Prints all unmarked numbers
    */
  public void print() {
    for (int i = 2; i < upperBound; i++) {
      if (sieve[i]) System.out.println(i);
    }
  }

  /**
    * Counts all unmarked numbers
    */
  public int count() {
    int ctr = 0;
    for (int i = 2; i < upperBound; i++) {
      if (sieve[i]) ctr++;
    }
    return ctr;
  }


}

