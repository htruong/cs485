/**
  * Parallel Quick Sort Thread
  * Written by Huan Truong <hnt7438@truman.edu>
  */
package PQSort;

import java.lang.Thread;

///////////////////////////////////////////////////////////////////////////////

public class TPSortThread extends Thread {
  private int start, finish;
  private SwappableList list;

  /** Construct and initialize a thread.
  /* @param name The name given to this thread
  */
  public TPSortThread (String name, int begin, int end, SwappableList sList) {
    setName (name);
    start = begin;
    finish = end;
    list = sList;
  }

  /** Begin execution of a TPSortThread.
  */
  public void run() {
    int left, right, pivot;

    if (start < finish) {
      left = start;
      right = finish + 1;

      pivot = start;
      do {
        do {
          left++;
        } while ((left < finish) && (list.get(left).compareTo(list.get(pivot)) < 0));

        do {
          right--;
        } while (list.get(right).compareTo(list.get(pivot)) > 0);

        if (left < right) {
          list.swap(left, right);
        }

      } while (left < right);

      list.swap(start, right);

      TPSortThread workerL
        = new TPSortThread(this.getName() + "_L", start, right - 1, list);
      TPSortThread workerR
        = new TPSortThread(this.getName() + "_R", right + 1, finish, list);

      // go go go
      workerL.start();
      workerR.start();

      // Wait for both of them
      try {
        workerL.join();
        workerR.join();
      } catch (Throwable e) {
        System.out.println ("Couldn't join with at least one of the two workers.");
      }
    }
  }
}

