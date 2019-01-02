/* a1691807 Yi-Ting, Hsieh */

import java.io.*;
import java.util.*;       
import java.io.File;
import java.io.IOException;
import java.nio.file.*;



public class SetUp {

    public static void main(String[] args) {

        System.out.println("Clean up all the directory");

        File file = new File("NA");
        deleteDirectory(file);
        file.mkdir();
        file = new File("NC");
        deleteDirectory(file);
        file.mkdir();
        file = new File("C");
        deleteDirectory(file);
        file.mkdir();
    }

    public static boolean deleteDirectory(File directoryToBeDeleted) {
        File[] allContents = directoryToBeDeleted.listFiles();
        if (allContents != null) {
            for (File file : allContents) {
                deleteDirectory(file);
            }
        }
        return directoryToBeDeleted.delete();
    }

}
