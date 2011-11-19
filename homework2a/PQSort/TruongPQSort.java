/**
  * Parallel Quick-Sort
  * Written by Huan Truong <hnt7438@truman.edu>
  */

package PQSort;

public class TruongPQSort {
  /**
    * Parallel-Quick Sort Java Implementation
    *
    * @param args Command line arguments
    */
  public static void main (String [] args) {
    SwappableList l = new SwappableList();

    try {
      l.loadList(args[0]);
    } catch (RuntimeException e) {
      System.out.println("The file is probably not found!");
    } catch (Throwable e) {
      System.out.println("There is something wrong!");
    }

    TPSortThread sThread = new TPSortThread("ChuckNorris", 0, l.size() - 1, l);

    sThread.start();

    try {
      sThread.join();
    } catch (Throwable e) {
      System.out.println ("Couldn't join with thread " + sThread.getName());
    }

    l.printList();
  }
}
