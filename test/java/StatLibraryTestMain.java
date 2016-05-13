import java.io.IOException;

import org.sequenceparser.EType;
import org.sequenceparser.Item;
import org.sequenceparser.ItemStat;

public class StatLibraryTestMain {
    public static void main(String[] args) throws IOException {
        if (args.length == 0) {
            System.out.println("You have to specify a path");
            return;
        }
        try {
            System.loadLibrary("sequenceParser-java");
            final String path = args[0];
            final Item item = new Item(EType.eTypeFolder, path);
            final ItemStat itemStat = new ItemStat(item);
            displayFolderContent(itemStat);
        } catch (Exception e) {
            System.out.println("Error");
            e.printStackTrace();
        }
    }

    private static void displayFolderContent(ItemStat itemStat) {
        System.out.println("=====================================");
        System.out.println("Size : " + itemStat.getSize());
        System.out.println("Real Size : " + itemStat.getRealSize());
        System.out.println("Size on disk : " + itemStat.getSizeOnDisk());
        System.out.println("Full Hard links : " + itemStat.getFullNbHardLinks());
        System.out.println("Hardlinks : " + itemStat.getNbHardLinks());
        System.out.println("Device : " + itemStat.getDeviceId());
    }
}

// How to use

// Compile on UNIX: javac -cp "/path/to/jar/sequenceParser-<version>.jar:." StatLibraryTestMain.java
// Run on UNIX: java -Djava.library.path=/path/to/lib/ -cp "/path/to/jar/sequenceParser-<version>.jar:." StatLibraryTestMain <input_path>

// Compile on Windows: javac -cp "/path/to/jar/sequenceParser-<version>.jar;." StatLibraryTestMain.java
// Run on Windows: java -Djava.library.path=/path/to/lib/ -cp "/path/to/jar/sequenceParser-<version>.jar;." StatLibraryTestMain <input_path>
