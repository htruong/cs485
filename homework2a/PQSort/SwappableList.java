/**
  * SwappableList
  * Written by Huan Truong <hnt7438@truman.edu>
  */
package PQSort;

import java.io.FileReader;
import java.io.BufferedReader;

import java.util.ArrayList;

import java.io.IOException;
import java.io.FileNotFoundException;
import java.lang.RuntimeException;

///////////////////////////////////////////////////////////////////////////////

public class SwappableList {

  private ArrayList<String> theList;

  /**
  * Creates a new SwappableList
  */
  public SwappableList() {
    theList = new ArrayList<String>();
  }

  /**
  * Loads a list from a file.
  * @param fileName The name of the file to load.
  */
  public void loadList(String fileName) throws IOException {
    try {
      FileReader fro = new FileReader(fileName);
      BufferedReader bro = new BufferedReader(fro);

      String theLine = bro.readLine();

      while (theLine != null) {
          theList.add(theLine);
          // read next line
          theLine = bro.readLine();
      }

      bro.close();
    } catch (FileNotFoundException exception) {
      // I do not know why you want RuntimeException here, should be FNFE?
      throw new RuntimeException(fileName + " does not exist. U mad?");
    } catch (IOException exception) {
      // Everything else
      throw new IOException(fileName + " has something wrong with it...");
    }
  }

  /**
  * Prints the entire list.
  */
  public void printList() {
    for (int i = 0; i < theList.size(); i++) {
      System.out.println(theList.get(i));
    }
  }

  /**
  * Returns the whole list's size
  * @return The list's size
  */
  public int size() {
    return theList.size();
  }

  /**
  * Get a specific position from the list
  * @param position: The position to get
  * @return The string at position
  */
  public String get(int position) throws RuntimeException {
    try {
      return theList.get(position);
    } catch (IndexOutOfBoundsException exception) {
      throw new RuntimeException("The index is out of bound.");
    }
  }

  /**
  * Set a specific position from the list
  * @param position: The position to replace
  * @param s: The string to set
  */
  public void set(String s, int position) throws RuntimeException {
    try {
      theList.set(position, s);
    } catch (IndexOutOfBoundsException exception) {
      throw new RuntimeException("The index is out of bound.");
    } catch (Throwable exception) { // FIXME: Sloppy programming
      throw new RuntimeException("Something is wrong...");
    }
  }

  /**
  * Swap two specific positions from the list
  * @param x: The first position to swap
  * @param y: The second position to swap
  */
  public void swap(int x, int y) throws RuntimeException {
    String swp;
    try {
      swp = theList.get(x);
      theList.set(x, theList.get(y));
      theList.set(y, swp);
    } catch (IndexOutOfBoundsException exception) {
      throw new RuntimeException("The index is out of bound.");
    } catch (Throwable exception) { // FIXME: Sloppy programming
      throw new RuntimeException("Something is wrong...");
    }
  }
}